#pragma once

#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/transaction.hpp>

#include <string>
#include <optional>

// eosio system
#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>
#include <eosio.system/get_trx_id.hpp>

// newdex public
#include <newdexpublic/newdexpublic.hpp>

// delphi oracle
#include <delphioracle/delphioracle.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("proxy")]] proxy : public contract {
public:
    using contract::contract;

    /**
     * Construct a new contract given the contract name
     *
     * @param {name} receiver - The name of this contract
     * @param {name} code - The code name of the action this contract is processing.
     * @param {datastream} ds - The datastream used
     */
    proxy( name receiver, name code, eosio::datastream<const char*> ds )
        : contract( receiver, code, ds ),
            _voters( get_self(), get_self().value ),
            _settings( get_self(), get_self().value ),
            _rewards( get_self(), get_self().value ),
            _referrals( get_self(), get_self().value ),
            _proxies( get_self(), get_self().value ),
            _staked( get_self(), get_self().value ),
            _redirect( get_self(), get_self().value ),
            _eosio_voters( "eosio"_n, "eosio"_n.value )
    {}

    /**
     * ## ACTION `signup`
     *
     * Signup owner to EOS Nation Proxy Staking Service
     *
     * - Authority: `owner` or `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner account
     * - `{name} [referral=""]` - (optional) referral account
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation signup '["myaccount", "tokenyield"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void signup( const eosio::name owner, std::optional<eosio::name> referral = ""_n );

    /**
     * ## ACTION `claim`
     *
     * Claim rewards from EOS Nation Proxy Staking Service
     *
     * - Authority: `owner`
     *
     * ### params
     *
     * - `{name} owner` - owner account
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation claim '["myaccount"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void claim( const eosio::name owner );

    /**
     * ## ACTION `unsignup`
     *
     * Remove owner from EOS Nation Proxy Staking Service
     *
     * - Authority: `owner`
     *
     * ### params
     *
     * - `{name} owner` - owner account
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation unsignup '["myaccount"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void unsignup( const eosio::name owner );

    /**
     * ## ACTION `refresh`
     *
     * Refresh voter information
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{name} voter` - voter account
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation refresh '["myaccount"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void refresh( const eosio::name voter );

    /**
     * ## ACTION `clean`
     *
     * Cleans contract tables
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{name} table` - table to clean
     * - `{name} [scope=""]` - (optional) scope to clean
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation clean '["voters", ""]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void clean( const eosio::name table, const std::optional<eosio::name> scope );

    /**
     * ## ACTION `setrate`
     *
     * Set APR rate
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{int64_t} [rate=400]` - APR rate (pips 1/100 of 1%)
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setrate '[400]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setrate( const int64_t rate = 400 );

    /**
     * ## ACTION `setinterval`
     *
     * Set claim interval in seconds
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{uint64_t} [interval=86400]` - claim interval in seconds
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setinterval '[86400]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setinterval( const int64_t interval = 86400 );

    /**
     * ## ACTION `setreward`
     *
     * Set authorized reward asset
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{symbol} sym` - reward token symbol
     * - `{name} contract` - reward token contract
     * - `{asset} price` - EOS price of reward
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setreward '["4,USDT", "tethertether", "0.3344 EOS"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setreward( const eosio::symbol sym, const eosio::name contract, const eosio::asset price );

    /**
     * ## ACTION `setportfolio`
     *
     * Set authorized reward asset
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{name} name` - portfolio name (used for scope)
     * - `{vector<symbol_code>} rewards` - reward token symbols
     * - `{vector<int64_t>} percentages` - reward percentages (pips 1/100 of 1%)
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setportfolio '["mixed", ["EOS", "USDT"], [5000, 5000]]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setportfolio( const eosio::name name, const std::vector<eosio::symbol_code> rewards, const std::vector<int64_t> percentages );

    /**
     * ## ACTION `setprice`
     *
     * Set price of rewards and re-calculate APR rate
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{symbol_code} code` - reward token symbol code
     * - `{asset} price` - EOS price of reward
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setprice '["USDT", "0.3344 EOS"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setprice( const eosio::symbol_code code, const eosio::asset price );

    /**
     * ## ACTION `setreferral`
     *
     * Set authorized referral
     *
     * - Authority: `get_self()` or `referral`
     *
     * ### params
     *
     * - `{name} name` - referral account name
     * - `{string} website` - referral website
     * - `{string} description` - referral description
     * - `{int64_t} [rate=50000]` - referral rate pips 1/100 of 1% (maximum of 5%)
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setreferral '["tokenyieldio", "Track and Manage Blockchain Rewards", "https://tokenyield.io", 50000]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setreferral( const eosio::name name, const string website, const string description, const int64_t rate );

    /**
     * ## ACTION `delreferral`
     *
     * Delete referral
     *
     * - Authority: `get_self()` or `referral`
     *
     * ### params
     *
     * - `{name} referral` - referral account name
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation delreferral '["tokenyieldio"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void delreferral( const eosio::name referral );

    /**
     * ## ACTION `setproxy`
     *
     * Set authorized proxy
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{name} proxy` - proxy account name
     * - `{bool} active` - true/false if proxy is active
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setproxy '["proxy4nation", true]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setproxy( const eosio::name proxy, const bool active );

    /**
     * ## ACTION `pause`
     *
     * Pause/unpause contract for maintenance
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{bool} paused` - true/false if contract is paused for maintenance
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation pause '[true]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void pause( const bool paused );

    /**
     * ## ACTION `setstaked`
     *
     * Allow owner to receive EOS rewards as staked instead of liquid.
     *
     * - Authority: `owner`
     *
     * ### params
     *
     * - `{name} owner` - owner account name
     * - `{bool} staked` - true/false to receiving EOS rewards as staked instead of liquid
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setstaked '["myaccount", true]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void setstaked( const eosio::name owner, const bool staked );

    /**
     * ## ACTION `setprices`
     *
     * Set prices of all rewards and re-calculate APR rate
     *
     * > Prices feed using NewDex & DelphiOracle
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setprices '[]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setprices();

    /**
     * ## ACTION `receipt`
     *
     * Push notification to owner with a receipt of incoming rewards
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner to be notified
     * - `{asset} staked` - staked amount of owner at the time of claim
     * - `{vector<asset>} rewards` - rewards earned during claim period
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation reset '["myaccount", "100.0000 EOS", ["0.0109 EOS"]]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void receipt( const eosio::name owner, const eosio::asset staked, const std::vector<eosio::asset> rewards );

    /**
     * ## ACTION `reset`
     *
     * Reset owner's next claim period
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner to reset next claim period
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation reset '["myaccount"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void reset( const eosio::name owner );

    /**
     * ## ACTION `setredirect`
     *
     * Redirect incoming rewards to another account
     *
     * - Authority: `owner`
     *
     * ### params
     *
     * - `{name} owner` - owner account name receiving rewards
     * - `{bool} to` - redirect rewards `to` account
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setredirect '["toaccount"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void setredirect( const eosio::name owner, const eosio::name to );

    /**
     * ## ACTION `claimall`
     *
     * Claim rewards from all voters
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation claimall '[]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void claimall( const uint64_t start, const uint64_t end );

    /**
     * ## ON_NOTIFY `transfer`
     *
     * On token transfer notification, update proxy APR
     *
     * ### example
     *
     * ```bash
     * cleos transfer eosnationftw proxy4nation "1.0000 EOS" "EOS Nation Proxy Staking Service"
     * ```
     */
    [[eosio::on_notify("*::transfer")]]
    void transfer( const name&    from,
                   const name&    to,
                   const asset&   quantity,
                   const string&  memo );

    [[eosio::on_notify("eosio::onerror")]]
    void onError();

    [[eosio::on_notify("eosio::delegatebw")]]
    void delegatebw( const name& from, const name& receiver,
                    const asset& stake_net_quantity, const asset& stake_cpu_quantity, bool transfer );

    [[eosio::on_notify("eosio::undelegatebw")]]
    void undelegatebw( const name& from, const name& receiver,
                    const asset& unstake_net_quantity, const asset& unstake_cpu_quantity );

    [[eosio::on_notify("eosio::buyrex")]]
    void buyrex( const name& from, const asset& amount );

    [[eosio::on_notify("eosio::sellrex")]]
    void sellrex( const name& from, const asset& rex );

    using claim_action = eosio::action_wrapper<"claim"_n, &proxy::claim>;
    using signup_action = eosio::action_wrapper<"signup"_n, &proxy::signup>;
    using refresh_action = eosio::action_wrapper<"refresh"_n, &proxy::refresh>;
    using setrate_action = eosio::action_wrapper<"setrate"_n, &proxy::setrate>;
    using setreferral_action = eosio::action_wrapper<"setreferral"_n, &proxy::setreferral>;
    using delreferral_action = eosio::action_wrapper<"delreferral"_n, &proxy::delreferral>;
    using setinterval_action = eosio::action_wrapper<"setinterval"_n, &proxy::setinterval>;
    using setstaked_action = eosio::action_wrapper<"setstaked"_n, &proxy::setstaked>;
    using setprice_action = eosio::action_wrapper<"setprice"_n, &proxy::setprice>;
    using setprices_action = eosio::action_wrapper<"setprices"_n, &proxy::setprices>;
    using receipt_action = eosio::action_wrapper<"receipt"_n, &proxy::receipt>;
    using reset_action = eosio::action_wrapper<"reset"_n, &proxy::reset>;
    using claimall_action = eosio::action_wrapper<"claimall"_n, &proxy::claimall>;
    using setredirect_action = eosio::action_wrapper<"setredirect"_n, &proxy::setredirect>;

private:
    /**
     * ## TABLE `rewards`
     *
     * - `{symbol} symbol` - reward token symbol
     * - `{name} contract` - reward token contract
     * - `{asset} price` - EOS price of reward
     *
     * ### example
     *
     * ```json
     * {
     *   "rows": [{
     *       "symbol": "4,EOS",
     *       "contract": "eosio.token",
     *       "price": "1.0000 EOS"
     *     },{
     *       "symbol": "4,DAPP",
     *       "contract": "dappservices",
     *       "price": "0.0050 EOS"
     *     },{
     *       "symbol": "4,USDT",
     *       "contract": "tethertether",
     *       "price": "0.3436 EOS"
     *     }
     *   ],
     *   "more": false
     * }
     * ```
     */
    struct [[eosio::table("rewards")]] rewards_row {
        eosio::symbol       symbol;
        eosio::name         contract;
        eosio::asset        price;

        uint64_t primary_key() const { return symbol.code().raw(); }
    };

    /**
     * ## TABLE `portfolio`
     *
     * - Scope: `owner`
     *
     * - `{symbol} symbol` - reward token symbol
     * - `{name} contract` - reward token contract
     * - `{int64_t} percentage` - reward percentage (pips 1/100 of 1%)
     *
     * ### example
     *
     * ```json
     * {
     *   "rows": [{
     *       "symbol": "4,EOS",
     *       "contract": "eosio.token",
     *       "percentage": 9000
     *     },{
     *       "symbol": "4,DAPP",
     *       "contract": "dappservices",
     *       "percentage": 250
     *     },{
     *       "symbol": "4,USDT",
     *       "contract": "tethertether",
     *       "percentage": 750
     *     }
     *   ],
     *   "more": false
     * }
     * ```
     */
    struct [[eosio::table("portfolio")]] portfolio_row {
        eosio::symbol       symbol;
        eosio::name         contract;
        int64_t             percentage;

        uint64_t primary_key() const { return symbol.code().raw(); }
    };

    /**
     * ## TABLE `voters`
     *
     * - `{name} owner` - owner staking to proxy
     * - `{name} proxy` - voting proxy
     * - `{vector<name>}` producers - the producers approved by this voter if no proxy set
     * - `{int64_t} staked` - voter info staked
     * - `{double} last_vote_weight` - last vote weight
     * - `{boolean} is_proxy` - true/false is proxy
     * - `{time_point_sec} next_claim_period` - next available claim period
     * - `{name} referral` - referral account
     *
     * ### example
     *
     * ```json
     * {
     *   "owner": "myaccount",
     *   "proxy": "proxy4nation",
     *   "staked": 6000,
     *   "last_vote_weight": "5361455468.19293689727783203",
     *   "is_proxy": 0,
     *   "next_claim_period": "2019-08-07T18:37:37",
     *   "referral": "tokenyield"
     * }
     * ```
     */
    struct [[eosio::table("voters")]] voters_row {
        eosio::name         owner;
        eosio::name         proxy;
        vector<eosio::name> producers;
        int64_t             staked = 0;
        double              last_vote_weight = 0;
        bool                is_proxy = false;
        time_point_sec      next_claim_period = time_point_sec(0);
        eosio::name         referral = ""_n;

        uint64_t primary_key() const { return owner.value; }
    };

    /**
     * ## TABLE `proxies`
     *
     * - `{name} proxy` - voting proxy
     * - `{bool} active` - true/false if proxy is active
     *
     * ### example
     *
     * ```json
     * {
     *   "proxy": "proxy4nation",
     *   "active": true
     * }
     * ```
     */
    struct [[eosio::table("proxies")]] proxies_row {
        eosio::name         proxy;
        bool                active = true;

        uint64_t primary_key() const { return proxy.value; }
    };

    /**
     * ## TABLE `referrals`
     *
     * - `{name} name` - referral account
     * - `{string} website` - referral website
     * - `{string} description` - referral description
     * - `{int64_t} [rate=50000]` - referral rate pips 1/100 of 1% (maximum of 5%)
     *
     * ### example
     *
     * ```json
     * {
     *   "name": "tokenyieldio",
     *   "website": "https://tokenyield.io",
     *   "description": "Track and Manage Blockchain Rewards",
     *   "rate": 50000
     * }
     * ```
     */
    struct [[eosio::table("referrals")]] referrals_row {
        eosio::name     name;
        string          website;
        string          description;
        int64_t         rate;

        uint64_t primary_key() const { return name.value; }
    };

    /**
     * ## TABLE `staked`
     *
     * - `{name} owner` - owner account name
     * - `{bool} staked` - true/false to receiving rewards as staked instead of liquid
     *
     * ### example
     *
     * ```json
     * {
     *   "proxy": "proxy4nation",
     *   "staked": true
     * }
     * ```
     */
    struct [[eosio::table("staked")]] staked_row {
        eosio::name     owner;
        bool            staked;

        uint64_t primary_key() const { return owner.value; }
    };


    /**
     * ## TABLE `redirect`
     *
     * - `{name} owner` - owner account name receiving rewards
     * - `{bool} to` - redirect rewards `to` account
     *
     * ### example
     *
     * ```json
     * {
     *   "owner": "myaccount",
     *   "to": "toaccount"
     * }
     * ```
     */
    struct [[eosio::table("redirect")]] redirect_row {
        eosio::name     owner;
        eosio::name     to;

        uint64_t primary_key() const { return owner.value; }
    };

    /**
     * ## TABLE `settings`
     *
     * - `{int64_t} [rate=400]` - APR rate pips 1/100 of 1%
     * - `{int64_t} [interval=86400]` - claim interval in seconds
     * - `{bool} [paused=false]` - true/false if contract is paused for maintenance
     *
     * ### example
     *
     * ```json
     * {
     *   "rate": 400,
     *   "interval": 86400,
     *   "paused": false
     * }
     * ```
     */
    struct [[eosio::table("settings")]] settings_row {
        int64_t rate = 400;
        int64_t interval = 86400;
        bool paused = false;
    };

    // Tables
    typedef eosio::multi_index< "voters"_n, voters_row> voters_table;
    typedef eosio::multi_index< "rewards"_n, rewards_row> rewards_table;
    typedef eosio::multi_index< "referrals"_n, referrals_row> referrals_table;
    typedef eosio::multi_index< "proxies"_n, proxies_row> proxies_table;
    typedef eosio::multi_index< "staked"_n, staked_row> staked_table;
    typedef eosio::multi_index< "redirect"_n, redirect_row> redirect_table;
    typedef eosio::multi_index< "portfolio"_n, portfolio_row> portfolio_table;
    typedef eosio::singleton< "settings"_n, settings_row> settings_table;

    // local instances of the multi indexes
    voters_table                    _voters;
    settings_table                  _settings;
    rewards_table                   _rewards;
    referrals_table                 _referrals;
    proxies_table                   _proxies;
    staked_table                    _staked;
    redirect_table                  _redirect;
    eosiosystem::voters_table       _eosio_voters;

    // private helpers
    void refresh_voter( const eosio::name owner );
    void refresh_claim_period( const eosio::name owner );
    int64_t calculate_amount( const eosio::symbol_code code, const int64_t staked, const int64_t multiplier, const int64_t rate, const int64_t interval );
    void require_auth_or_self( eosio::name owner );
    void require_auth_or_self_or_referral( eosio::name owner );
    void send_referral( const eosio::name owner, const eosio::asset quantity, const eosio::name contract );

    // proxies
    eosio::name get_voter_proxy( const eosio::name owner );
    eosio::name get_active_proxy();
    bool available_proxy( const eosio::name owner );
    void check_available_proxy( const eosio::name owner );
    void check_active_proxy( const eosio::name owner );

    // deferred
    void auto_refresh( const eosio::name owner );
    void auto_claim( const eosio::name owner );
    void auto_setprices();

    // claim
    void stake_to( const eosio::name receiver, const int64_t amount );
    void rex_to( const eosio::name receiver, const int64_t amount );
    std::vector<eosio::asset> send_rewards( const eosio::name owner, const int64_t staked );
    void send_reward( const eosio::name owner, const eosio::asset quantity, const eosio::name contract );

    // settings
    void check_pause();

    // price
    eosio::asset get_usdt_price();
    eosio::asset get_dapp_price();
    void update_price( eosio::symbol_code code, const eosio::asset price );

    // staked
    bool is_staked( const eosio::name owner );

    // redirect
    eosio::name is_redirect( const eosio::name owner );

    // portfolio
    void clear_portfolio( const eosio::name owner );
    void set_portfolio_reward( const eosio::name owner, const eosio::symbol_code reward, const int64_t percentage );
    eosio::name has_portfolio( const eosio::name owner );
    void update_reward_percentage( const eosio::symbol_code code, const int64_t percentage );
    double get_current_price( const uint64_t pair_id );
};
