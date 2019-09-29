#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>

#include <string>
#include <optional>

#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>
#include <eosio.system/get_trx_id.hpp>

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
            _last( get_self(), get_self().value ),
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
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation clean '["voters"]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void clean( const eosio::name table );

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
     * Set reward asset
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{extended_symbol} reward` - token reward symbol
     * - `{int64_t} multiplier` - token reward multiplier
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setreward '[{"contract":"eosio.token", "symbol": "4,EOS"}, 1]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setreward( const extended_symbol reward, const int64_t multiplier );

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
     * ## ACTION `setrex`
     *
     * Set REX APR rate
     *
     * - Authority: `get_self()`
     *
     * ### params
     *
     * - `{int64_t} [rate=16]` - REX APR rate (pips 1/100 of 1%)
     *
     * ### example
     *
     * ```bash
     * cleos push action proxy4nation setrex '[16]' -p proxy4nation
     * ```
     */
    [[eosio::action]]
    void setrex( const int64_t rate = 16 );

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

private:

    /**
     * ## TABLE `rewards`
     *
     * - `{symbol} symbol` - reward token symbol
     * - `{name} contract` - reward token contract
     * - `{int64_t} multiplier` - reward multiplier
     *
     * ### example
     *
     * ```json
     * {
     *   "symbol": "4,EOS",
     *   "contract": "eosio.token",
     *   "multiplier": 1
     * }
     * ```
     */
    struct [[eosio::table("rewards")]] rewards_row {
        eosio::symbol       symbol;
        eosio::name         contract;
        int64_t             multiplier;

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
     * ## TABLE `settings`
     *
     * - `{int64_t} [rate=400]` - APR rate pips 1/100 of 1%
     * - `{int64_t} [interval=86400]` - claim interval in seconds
     * - `{int64_t} [rex=16]` - REX APR rate pips 1/100 of 1%
     *
     * ### example
     *
     * ```json
     * {
     *   "rate": 400,
     *   "interval": 86400,
     *   "rex": 16
     * }
     * ```
     */
    struct [[eosio::table("settings")]] settings_row {
        int64_t rate = 400;
        int64_t interval = 86400;
        int64_t rex = 16;
    };

    /**
     * ## TABLE `last`
     *
     * - `{name} voter` - last voter to claim rewards
     * - `{asset} amount` - last claimed reward amount
     * - `{int64_t} rate` - APR rate pips 1/100 of 1%
     * - `{int64_t} interval` - claim interval in seconds
     * - `{time_point_sec} timestamp` - timestamp of last claim
     * - `{checksum256} trx_id` - transaction id
     *
     * ### example
     *
     * ```json
     * {
     *   "voter": "myaccount",
     *   "amount": "1.5000 EOS",
     *   "rate": 350,
     *   "interval": 86400,
     *   "timestamp": "2019-09-23T00:00:00",
     *   "trx_id": "0e90ad6152b9ba35500703bc9db858f6e1a550b5e1a8de05572f81cdcaae3a08"
     * }
     * ```
     */
    struct [[eosio::table("last")]] last_row {
        eosio::name voter;
        eosio::asset amount;
        int64_t rate;
        int64_t interval;
        eosio::time_point_sec timestamp;
        eosio::checksum256 trx_id;
    };

    // Tables
    typedef eosio::multi_index< "voters"_n, voters_row> voters_table;
    typedef eosio::multi_index< "rewards"_n, rewards_row> rewards_table;
    typedef eosio::multi_index< "referrals"_n, referrals_row> referrals_table;
    typedef eosio::singleton< "settings"_n, settings_row> settings_table;
    typedef eosio::singleton< "last"_n, last_row> last_table;

    // local instances of the multi indexes
    voters_table                _voters;
    settings_table              _settings;
    last_table                  _last;
    rewards_table               _rewards;
    referrals_table             _referrals;
    eosiosystem::voters_table   _eosio_voters;

    // private helpers
    void refresh_voter( const eosio::name owner );
    void refresh_claim_period( const eosio::name owner );
    void update_last( const eosio::name voter, const eosio::asset amount, const int64_t rate, const int64_t interval );
    void send_rewards( const eosio::name owner, const int64_t staked, const eosio::symbol sym );
    void check_proxy( const eosio::name proxy );
    int64_t calculate_amount( const int64_t staked, const int64_t multiplier, const int64_t rate, const int64_t interval );
    void require_auth_or_self( eosio::name owner );
    void require_auth_or_self_or_referral( eosio::name owner );
    void send_referral( const eosio::name owner, const eosio::asset quantity, const eosio::name contract );
};
