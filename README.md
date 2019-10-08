# EOS Nation - Proxy Staking Service

## ACTION

- [`claim`](#action-claim)
- [`signup`](#action-signup)
- [`unsignup`](#action-unsignup)
- [`refresh`](#action-refresh)
- [`setrate`](#action-setrate)
- [`setinterval`](#action-setinterval)
- [`setreward`](#action-setreward)
- [`setprice`](#action-setprice)
- [`setreferral`](#action-setreferral)
- [`setproxy`](#action-setproxy)
- [`delreferral`](#action-delreferral)
- [`pause`](#action-pause)
- [`clean`](#action-clean)

## TABLE

- [`rewards`](#table-rewards)
- [`settings`](#table-settings)
- [`voters`](#table-voters)
- [`referrals`](#table-referrals)
- [`proxies`](#table-proxies)

## APR Formula

```c++
// amount = Staked * APR / 4 precision / 365 days * multiplier / (24 hours / interval)
int64_t amount = staked * rate / 10000 / 365 * multiplier / ( 86400 / interval );
```

**Example**

User has 10K EOS staked and claims his 24 hour reward will earn `1.0547 EOS`

```js
Staked * APR / 365 days = reward
10000.0000 * 0.0385 / 365 = 1.0547
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
- `{int64_t} multiplier` - token reward multiplier
- `{asset} price` - EOS price of reward

### example

```bash
cleos push action proxy4nation setreward '["4,USDT", "tethertether", 1, "0.3344 EOS","]' -p proxy4nation
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

## ACTION `setreferral`

Set authorized referral

- Authority: `get_self()` or `referral`

### params

- `{name} name` - referral account name
- `{string} website` - referral website
- `{string} description` - referral description
- `{int64_t} [rate=50000]` - referral rate pips 1/100 of 1% (maximum of 5%)

### example

```bash
cleos push action proxy4nation setreferral '["tokenyieldio", "https://tokenyield.io", "Track and Manage Blockchain Rewards", 50000]' -p proxy4nation
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

## ACTION `clean`

Cleans contract tables

- Authority: `get_self()`

### params

- `{name} table` - table to clean

### example

```bash
cleos push action proxy4nation clean '["referrals"]' -p proxy4nation
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

## TABLE `rewards`

- `{symbol} symbol` - reward token symbol
- `{name} contract` - reward token contract
- `{int64_t} multiplier` - reward multiplier
- `{asset} price` - EOS price of reward
- `{int64_t} apr` - APR rate pips 1/100 of 1%

### example

```json
{
  "symbol": "4,EOS",
  "contract": "eosio.token",
  "multiplier": 1,
  "price": "0.3344 EOS",
  "apr": 70
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
- `{int64_t} [rate=50000]` - referral rate pips 1/100 of 1% (maximum of 5%)

### example

```json
{
  "name": "tokenyieldio",
  "website": "https://tokenyield.io",
  "description": "Track and Manage Blockchain Rewards",
  "rate": 50000
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
