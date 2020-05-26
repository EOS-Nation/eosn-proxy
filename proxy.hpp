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
#include <eosio.system/exchange_state.hpp>
#include <eosio.system/eosio.system.hpp>
#include <eosio.system/get_trx_id.hpp>

// newdex public
#include <newdexpublic/newdexpublic.hpp>

// delphi oracle
#include <delphioracle/delphioracle.hpp>

static constexpr int64_t DAY = 86400; // 24 hours

using namespace eosio;
using namespace std;

class [[eosio::contract("proxy")]] proxy : public contract {
public:
    using contract::contract;

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
        symbol       symbol;
        name                contract;
        asset        price;

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
        symbol       symbol;
        name                contract;
        int64_t             percentage;

        uint64_t primary_key() const { return symbol.code().raw(); }
    };

    /**
     * ## TABLE `portfolio`
     *
     * - `{name} owner` - owner's portfolio
     * - `{vector<symbol_code>} rewards` - reward token symbols
     * - `{vector<int64_t>} percentages` - reward percentages (pips 1/100 of 1%)
     *
     * ### example
     *
     * ```json
     * {
     *   "owner": "myaccount",
     *   "rewards": ["EOS", "USDT"],
     *   "percentages": [9000, 1000]
     * }
     * ```
     */
    struct [[eosio::table("portfolio2")]] portfolio2_row {
        name                    owner;
        vector<symbol_code>     rewards;
        vector<int64_t>         percentages;

        uint64_t primary_key() const { return owner.value; }
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
        name                owner;
        name                proxy;
        vector<name>        producers;
        int64_t             staked = 0;
        double              last_vote_weight = 0;
        bool                is_proxy = false;
        time_point_sec      next_claim_period = time_point_sec(0);
        name                referral = ""_n;

        uint64_t primary_key() const { return owner.value; }
    };

    /**
     * ## TABLE `v2.voters`
     *
     * - `{name} owner` - owner staking to proxy
     * - `{time_point_sec} next_claim_period` - next available claim period
     * - `{int64_t} staked` - voter info staked
     * - `{name} referral` - referral account
     * - `{set<symbol_code>} rewards` - (default ["EOS"]) receiving reward token
     * - `{map<name, bool>} protocol_features` - (true/false) activated protocol features
     *
     *
     * ### example
     *
     * ```json
     * {
     *   "owner": "myaccount",
     *   "next_claim_period": "2019-08-07T18:37:37",
     *   "staked": 20049272,
     *   "referral": "tokenyield",
     *   "rewards": ["EOS", "DAPP"],
     *   "protocol_features": [
     *      {"key": "staked", "value": true}
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("voters.v2")]] voters_v2_row {
        name                    owner;
        time_point_sec          next_claim_period = time_point_sec(0);
        int64_t                 staked = 0;
        name                    referral = ""_n;
        set<symbol_code>        rewards = set<symbol_code>{symbol_code{"EOS"}};
        map<name, bool>         protocol_features;

        uint64_t primary_key() const { return owner.value; }
        uint64_t by_next_claim() const { return next_claim_period.sec_since_epoch(); }
        uint64_t by_referral() const { return referral.value; }
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
        name            proxy;
        bool            active = true;

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
     *   "rate": 500
     * }
     * ```
     */
    struct [[eosio::table("referrals")]] referrals_row {
        name            name;
        string          website;
        string          description;
        int64_t         rate;

        uint64_t primary_key() const { return name.value; }
    };

    /**
     * ## TABLE `referrals.v2`
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
     *   "referral": "tokenyieldio",
     *   "metadata_json": [
     *     {"key": "website", "value": "https://tokenyield.io"},
     *     {"key": "description", "value": "Track and Manage Blockchain Rewards"}
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("referrals.v2")]] referrals_v2_row {
        name                    referral;
        map<name, string>       metadata_json;

        uint64_t primary_key() const { return referral.value; }
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
        name        owner;
        bool        staked;

        uint64_t primary_key() const { return owner.value; }
    };

    /**
     * ## TABLE `settings`
     *
     * - `{int64_t} [rate=185]` - APR rate pips 1/100 of 1%
     * - `{int64_t} [referral_rate=500]` - referral rate pips 1/100 of 1% (maximum of 5%)
     * - `{int64_t} [interval=86400]` - claim interval in seconds
     * - `{bool} [paused=false]` - true/false if contract is paused for maintenance
     *
     * ### example
     *
     * ```json
     * {
     *   "rate": 185,
     *   "referral_rate": 500,
     *   "interval": 86400,
     *   "paused": false
     * }
     * ```
     */
    struct [[eosio::table("settings")]] settings_row {
        int64_t rate = 185;
        int64_t referral_rate = 500;
        int64_t interval = 86400;
        bool paused = false;
    };

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
            _portfolio2( get_self(), get_self().value ),
            _eosio_voters( "eosio"_n, "eosio"_n.value ),
            _rexpool( "eosio"_n, "eosio"_n.value )
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
    void signup( const name owner, std::optional<name> referral = ""_n );

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
    void claim( const name owner );

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
    void unsignup( const name owner );

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
    void refresh( const name voter );

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
    void clean( const name table, const std::optional<name> scope );

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
    void setreward( const symbol sym, const name contract, const asset price );

    /**
     * ## ACTION `delreward`
     *
     * Delete authorized reward asset
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{symbol_code} sym_code` - reward token symbol code
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation delreward '["USDT"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void delreward( const symbol_code sym_code );

    /**
     * ## ACTION `setportfolio`
     *
     * Set owner's portfolio reward allocation
     *
     * - Authority: `owner` or `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner's portfolio
     * - `{vector<symbol_code>} rewards` - reward token symbols
     * - `{vector<int64_t>} percentages` - reward percentages (pips 1/100 of 1%)
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setportfolio '["myaccount", ["EOS", "USDT"], [9000, 1000]]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void setportfolio( const name owner, const std::vector<symbol_code> rewards, const std::vector<int64_t> percentages );

    /**
     * ## ACTION `delportfolio`
     *
     * Delete owner's portfolio
     *
     * - Authority: `owner` or `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner of portfolio
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation delportfolio '["myaccount"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void delportfolio( const name owner );

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
    void setprice( const symbol_code code, const asset price );

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
     * - `{int64_t} [rate=500]` - referral rate pips 1/100 of 1% (maximum of 5%)
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setreferral '["tokenyieldio", "Track and Manage Blockchain Rewards", "https://tokenyield.io", 500]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setreferral( const name name, const string website, const string description, const int64_t rate );

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
    void delreferral( const name referral );

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
    void setproxy( const name proxy, const bool active );

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
    void setstaked( const name owner, const bool staked );

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
    void receipt( const name owner, const asset staked, const std::vector<asset> rewards );

    [[eosio::action]]
    void payforcpu( optional<permission_level> payer );

    [[eosio::action]]
    void setparams( const optional<settings_row> params );

    [[eosio::action]]
    void migrate( const name owner );

    [[eosio::action]]
    void migrateall( uint64_t skip );

    [[eosio::on_notify("*::transfer")]]
    void transfer( const name&    from,
                   const name&    to,
                   const asset&   quantity,
                   const string&  memo );

    using claim_action = eosio::action_wrapper<"claim"_n, &proxy::claim>;
    using signup_action = eosio::action_wrapper<"signup"_n, &proxy::signup>;
    using refresh_action = eosio::action_wrapper<"refresh"_n, &proxy::refresh>;
    using setrate_action = eosio::action_wrapper<"setrate"_n, &proxy::setrate>;
    using setreferral_action = eosio::action_wrapper<"setreferral"_n, &proxy::setreferral>;
    using delreferral_action = eosio::action_wrapper<"delreferral"_n, &proxy::delreferral>;
    using setstaked_action = eosio::action_wrapper<"setstaked"_n, &proxy::setstaked>;
    using setprice_action = eosio::action_wrapper<"setprice"_n, &proxy::setprice>;
    using setprices_action = eosio::action_wrapper<"setprices"_n, &proxy::setprices>;
    using receipt_action = eosio::action_wrapper<"receipt"_n, &proxy::receipt>;
    using setportfolio_action = eosio::action_wrapper<"setportfolio"_n, &proxy::setportfolio>;
    using delportfolio_action = eosio::action_wrapper<"delportfolio"_n, &proxy::delportfolio>;
    using setreward_action = eosio::action_wrapper<"setreward"_n, &proxy::setreward>;

private:
    // Tables
    typedef eosio::multi_index< "voters"_n, voters_row> voters_table;
    typedef eosio::multi_index< "rewards"_n, rewards_row> rewards_table;
    typedef eosio::multi_index< "referrals"_n, referrals_row> referrals_table;
    typedef eosio::multi_index< "proxies"_n, proxies_row> proxies_table;
    typedef eosio::multi_index< "staked"_n, staked_row> staked_table;
    typedef eosio::multi_index< "portfolio"_n, portfolio_row> portfolio_table;
    typedef eosio::multi_index< "portfolio2"_n, portfolio2_row> portfolio2_table;
    typedef eosio::singleton< "settings"_n, settings_row> settings_table;

    // Tables v2
    typedef eosio::multi_index< "voters.v2"_n, voters_v2_row,
		indexed_by<"bynextclaim"_n, const_mem_fun<voters_v2_row, uint64_t, &voters_v2_row::by_next_claim>>,
        indexed_by<"byreferral"_n, const_mem_fun<voters_v2_row, uint64_t, &voters_v2_row::by_referral>>
	> voters_v2_table;

    typedef eosio::multi_index< "referrals.v2"_n, referrals_v2_row> referrals_v2_table;

    // local instances of the multi indexes
    voters_v2_table                 _voters;
    settings_table                  _settings;
    rewards_table                   _rewards;
    referrals_table                 _referrals;
    proxies_table                   _proxies;
    staked_table                    _staked;
    portfolio2_table                _portfolio2;
    eosiosystem::voters_table       _eosio_voters;
    eosiosystem::rex_pool_table     _rexpool;

    // refresh
    void update_voter_staked( const name owner );
    void erase_ineligible( const name owner );
    void refresh_claim_period( const name owner );

    // utils
    void check_voter_exists( const name owner );

    // claim
    int64_t calculate_amount( const symbol_code sym_code, const int64_t staked, const int64_t multiplier, const int64_t rate, const int64_t interval );
    void send_referral( const name owner, const asset quantity, const name contract );

    // proxies
    name get_voter_proxy( const name owner );
    name get_active_proxy();
    bool available_proxy( const name owner );
    void check_available_proxy( const name owner );
    void check_active_proxy( const name owner );

    // // deferred
    // void auto_refresh( const name owner );
    // void auto_claim( const name owner );
    // void auto_setprices();

    // claim
    void stake_to( const name receiver, const int64_t amount );
    void rex_to( const name receiver, const int64_t amount );
    std::vector<asset> send_rewards( const name owner, const int64_t staked );
    void send_reward( const name owner, const asset quantity, const name contract );

    // settings
    void check_pause();

    // price
    asset get_usdt_price();
    asset get_dapp_price();
    void update_price( symbol_code sym_code, const asset price );
    void update_price( const symbol_code sym_code );
    void update_reward_price( const symbol_code sym_code );

    // staked
    bool is_staked( const name owner );

    // rewards
    void check_reward_exists( const symbol_code sym_code );

    // portfolio
    void set_portfolio_rewards( const name owner, const std::vector<symbol_code> rewards, const std::vector<int64_t> percentages );
    name has_portfolio( const name owner );
    void update_reward_percentage( const symbol_code code, const int64_t percentage );
    double get_current_price( const uint64_t pair_id );

    // delegatebw
    asset get_delegatebw( const name owner );
    asset claim_delegatebw( const name owner );
    int64_t get_rented_tokens( const asset payment );

    // TO ERASE (Backward compatible)
    void clear_portfolio( const name owner );
    void set_portfolio_reward( const name owner, const symbol_code reward, const int64_t percentage );

    // signup
    void check_already_signup( const name owner, std::optional<name> referral );

    // // on_notify
    // void send_deferred( const eosio::action action, const uint64_t key, const uint64_t interval );
};
