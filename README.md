# EOS Nation - StrengthEARN the network `proxy4nation`

## ACTION

### USER ACTIONS

- [`claim`](#action-claim)
- [`signup`](#action-signup)
- [`unsignup`](#action-unsignup)
- [`setstaked`](#action-setstaked)
- [`setredirect`](#action-setredirect)
- [`setportfolio`](#action-setportfolio)

### REFERRAL ACTION

- [`claim`](#action-claim)
- [`setreferral`](#action-setreferral)
- [`delreferral`](#action-delreferral)

### ADMIN ACTIONS

- [`setrate`](#action-setrate)
- [`setinterval`](#action-setinterval)
- [`setreward`](#action-setreward)
- [`delreward`](#action-delreward)
- [`setprice`](#action-setprice)
- [`setproxy`](#action-setproxy)
- [`setreferral`](#action-setreferral)
- [`delreferral`](#action-delreferral)
- [`setprices`](#action-setprices)
- [`receipt`](#action-receipt)
- [`refresh`](#action-refresh)
- [`reset`](#action-reset)
- [`pause`](#action-pause)
- [`clean`](#action-clean)
- [`claimall`](#action-claimall)

## TABLE

- [`rewards`](#table-rewards)
- [`settings`](#table-settings)
- [`voters`](#table-voters)
- [`referrals`](#table-referrals)
- [`proxies`](#table-proxies)

## APR Formula

```c++
// amount = Staked * APR Rate / 365 days * percentage / (24 hours / interval) * price
int64_t amount = staked * rate / 10000.0 / 365.0 * percentage / 10000.0 / ( 86400.0 / interval ) * (10000.0 / price.amount);
```

**Example**

User has 10K EOS staked and claims his 24 hour reward (at 2.8% APR) will earn `0.7671 EOS`

```js
Staked * APR / 365 days = reward
10000.0000 * 0.028 / 365 = 0.7671
```

## ACTION `signup`

Signup owner to EOS Nation Proxy Staking Service

- Authority: `owner` or `get_self()`

### params

- `{name} owner` - owner account
- `{name} [referral=""]` - (optional) referral account

### example

```bash
cleos push action proxy4nation signup '["myaccount", "tokenyieldio"]' -p myaccount
```

## ACTION `claim`

Claim rewards from EOS Nation Proxy Staking Service

- Authority: `owner` or `referral` or `get_self()`

### params

- `{name} owner` - owner account

### example

```bash
cleos push action proxy4nation claim '["myaccount"]' -p myaccount
```

## ACTION `unsignup`

Remove owner from EOS Nation Proxy Staking Service

- Authority: `owner`

### params

- `{name} owner` - owner account

### example

```bash
cleos push action proxy4nation unsignup '["myaccount"]' -p myaccount
```

## ACTION `refresh`

Refresh voter information

- Authority: `any`

### params

- `{name} voter` - voter account

### example

```bash
cleos push action proxy4nation refresh '["myaccount"]' -p proxy4nation
```

## ACTION `setrate`

Set APR rate

- Authority: `get_self()`

### params

- `{int64_t} [rate=400]` - APR rate (pips 1/100 of 1%)

### example

```bash
cleos push action proxy4nation setrate '[400]' -p proxy4nation
```

## ACTION `setinterval`

Set claim interval in seconds

- Authority: `get_self()`

### params

- `{int64_t} [interval=86400]` - claim interval in seconds

### example

```bash
cleos push action proxy4nation setinterval '[86400]' -p proxy4nation
```

## ACTION `setreward`

Set authorized reward asset

- Authority: `get_self()`

### params

- `{symbol} sym` - reward token symbol
- `{name} contract` - reward token contract
- `{asset} price` - EOS price of reward

### example

```bash
cleos push action proxy4nation setreward '["4,USDT", "tethertether", "0.3344 EOS"]' -p proxy4nation
```

## ACTION `delreward`

Delete authorized reward asset

- Authority: `get_self()`

### params

- `{symbol_code} sym_code` - reward token symbol code

### example

```bash
cleos push action proxy4nation delreward '["USDT"]' -p proxy4nation
```

## ACTION `setportfolio`

Set owner's portfolio reward allocation

- Authority: `owner`

### params

- `{name} owner` - owner's portfolio
- `{vector<symbol_code>} rewards` - reward token symbols
- `{vector<int64_t>} percentages` - reward percentages (pips 1/100 of 1%)

### example

```bash
cleos push action proxy4nation setportfolio '["myaccount", ["EOS", "USDT"], [5000, 5000]]' -p myaccount
```

## ACTION `setprice`

Set price of rewards and re-calculate APR rate

- Authority: `get_self()`

### params

- `{symbol_code} code` - reward token symbol code
- `{asset} price` - EOS price of reward

### example

```bash
cleos push action proxy4nation setprice '["USDT", "0.3344 EOS"]' -p proxy4nation
```

## ACTION `setprices`

Set prices of all rewards and re-calculate APR rate

> Prices feed using NewDex & DelphiOracle

- Authority: `get_self()`

### params

### example

```bash
cleos push action proxy4nation setprices '[]' -p proxy4nation
```

## ACTION `setreferral`

Set authorized referral

- Authority: `get_self()` or `referral`

### params

- `{name} name` - referral account name
- `{string} website` - referral website
- `{string} description` - referral description
- `{int64_t} [rate=500]` - referral rate pips 1/100 of 1% (maximum of 5%)

### example

```bash
cleos push action proxy4nation setreferral '["tokenyieldio", "https://tokenyield.io", "Track and Manage Blockchain Rewards", 500]' -p proxy4nation
```

## ACTION `delreferral`

Delete referral

- Authority: `get_self()` or `referral`

### params

- `{name} referral` - referral account name

### example

```bash
cleos push action proxy4nation delreferral '["tokenyieldio"]' -p proxy4nation
```

## ACTION `setproxy`

Set authorized proxy

- Authority: `get_self()`

### params

- `{name} proxy` - proxy account name
- `{bool} active` - true/false if proxy is active

### example

```bash
cleos push action proxy4nation setproxy '["proxy4nation", true]' -p proxy4nation
```

## ACTION `setstaked`

Allow owner to receive EOS rewards as staked instead of liquid.

- Authority: `owner`

### params

- `{name} owner` - owner account name
- `{bool} staked` - true/false to receiving EOS rewards as staked instead of liquid

### example

```bash
cleos push action proxy4nation setstaked '["myaccount", true]' -p myaccount
```

## ACTION `setredirect`

Redirects incoming rewards to another account

- Authority: `owner`

### params

- `{name} owner` - owner account name receiving rewards
- `{bool} to` - redirect rewards `to` account

### example

```bash
cleos push action proxy4nation setredirect '["toaccount"]' -p myaccount
```

## ACTION `receipt`

Push notification to owner with a receipt of incoming rewards

- Authority: `get_self()`

### params

- `{name} owner` - owner to be notified
- `{asset} staked` - staked amount of owner at the time of claim
- `{vector<asset>} rewards` - rewards earned during claim period

### example

```bash
cleos push action proxy4nation receipt '["myaccount", "100.0000 EOS", ["0.0109 EOS"]]' -p proxy4nation
```

## ACTION `reset`

Reset owner's next claim period

- Authority: `get_self()`

### params

- `{name} owner` - owner to reset next claim period

### example

```bash
cleos push action proxy4nation reset '["myaccount"]' -p proxy4nation
```

## ACTION `clean`

Cleans contract tables

- Authority: `get_self()`

### params

- `{name} table` - table to clean
- `{name} [scope=""]` - (optional) scope to clean

### example

```bash
cleos push action proxy4nation clean '["referrals", ""]' -p proxy4nation
```

## ACTION `pause`

Pause/unpause contract for maintenance

- Authority: `get_self()`

### params

- `{bool} paused` - true/false if contract is paused for maintenance

### example

```bash
cleos push action proxy4nation pause '[true]' -p proxy4nation
```

## ACTION `claimall`

Claim rewards from all voters

- Authority: `get_self()`

### params

### example

```bash
cleos push action proxy4nation claimall '[]' -p proxy4nation
```

## TABLE `rewards`

- `{symbol} symbol` - reward token symbol
- `{name} contract` - reward token contract
- `{asset} price` - EOS price of reward

### example

```json
{
  "rows": [{
      "symbol": "4,EOS",
      "contract": "eosio.token",
      "price": "1.0000 EOS"
    },{
      "symbol": "4,DAPP",
      "contract": "dappservices",
      "price": "0.0050 EOS"
    },{
      "symbol": "4,USDT",
      "contract": "tethertether",
      "price": "0.3436 EOS"
    }
  ],
  "more": false
}
```

## TABLE `voters`

- `{name} owner` - owner staking to proxy
- `{name} proxy` - voting proxy
- `{vector<name>}` producers - the producers approved by this voter if no proxy set
- `{int64_t} staked` - voter info staked
- `{double} last_vote_weight` - last vote weight
- `{boolean} is_proxy` - true/false is proxy
- `{time_point_sec} next_claim_period` - next available claim period
- `{name} referral` - referral account

### example

```json
{
  "owner": "myaccount",
  "proxy": "proxy4nation",
  "producers": [],
  "staked": 6000,
  "last_vote_weight": "5361455468.19293689727783203",
  "is_proxy": 0,
  "next_claim_period": "2019-08-07T18:37:37",
  "referral": "tokenyieldio"
}
```

## TABLE `referrals`

- `{name} name` - referral account
- `{string} website` - referral website
- `{string} description` - referral description
- `{int64_t} [rate=500]` - referral rate pips 1/100 of 1% (maximum of 5%)

### example

```json
{
  "name": "tokenyieldio",
  "website": "https://tokenyield.io",
  "description": "Track and Manage Blockchain Rewards",
  "rate": 500
}
```

## TABLE `settings`

- `{int64_t} [rate=350]` - APR rate pips 1/100 of 1%
- `{int64_t} [interval=86400]` - claim interval in seconds
- `{bool} [paused=false]` - true/false if contract is paused for maintenance

### example

```json
{
  "rate": 350,
  "interval": 86400,
  "paused": false
}
```

## TABLE `proxies`

- `{name} proxy` - proxy account name
- `{bool} active` - true/false if proxy is active

### example

```json
{
  "proxy": "proxy4nation",
  "active": true
}
```

## TABLE `staked`

- `{name} owner` - owner account name
- `{bool} staked` - true/false to receiving rewards as staked instead of liquid

### example

```json
{
  "proxy": "proxy4nation",
  "staked": true
}
```

## TABLE `redirect`

- `{name} owner` - owner account name receiving rewards
- `{bool} to` - redirect rewards `to` account

### example

```json
{
  "owner": "myaccount",
  "to": "toaccount"
}
```

## TABLE `portfolio`

- Scope: `owner`

- `{symbol} symbol` - reward token symbol
- `{name} contract` - reward token contract
- `{int64_t} percentage` - reward percentage (pips 1/100 of 1%)

### example

```json
{
  "rows": [{
      "symbol": "4,EOS",
      "contract": "eosio.token",
      "percentage": 9000
    },{
      "symbol": "4,DAPP",
      "contract": "dappservices",
      "percentage": 250
    },{
      "symbol": "4,USDT",
      "contract": "tethertether",
      "percentage": 750
    }
  ],
  "more": false
}
```
