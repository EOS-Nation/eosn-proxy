# EOS Nation - Proxy Staking Service

## ACTION

- [`claim`](#action-claim)
- [`signup`](#action-signup)
- [`refresh`](#action-refresh)
- [`setrate`](#action-setrate)
- [`setinterval`](#action-setinterval)
- [`setreward`](#action-setreward)
- [`setreferral`](#action-setreferral)
- [`setrex`](#action-setrex)

## TABLE

- [`rewards`](#table-rewards)
- [`settings`](#table-settings)
- [`voters`](#table-voters)
- [`referrals`](#table-referrals)
- [`last`](#table-last)

## ACTION `signup`

Signup owner to EOS Nation Proxy Staking Service

- Authority: `owner`

### params

- `{name} owner` - owner account
- `{name} [referral=""]` - (optional) referral account

### example

```bash
cleos push action proxy4nation signup '["myaccount", "tokenyieldio"]' -p myaccount
```

## ACTION `claim`

Claim rewards from EOS Nation Proxy Staking Service

- Authority: `owner`

### params

- `{name} owner` - owner account

### example

```bash
cleos push action proxy4nation claim '["myaccount"]' -p myaccount
```

## ACTION `refresh`

Refresh voter information

- Authority: `get_self()`

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

## ACTION `setrex`

Set REX APR rate

- Authority: `get_self()`

### params

- `{int64_t} [rate=16]` - REX APR rate (pips 1/100 of 1%)

### example

```bash
cleos push action proxy4nation setrex '[16]' -p proxy4nation
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

- `{extended_symbol} reward` - token reward symbol
- `{int64_t} multiplier` - token reward multiplier

### example

```bash
cleos push action proxy4nation setreward '[{"contract":"eosio.token", "symbol": "4,EOS"}, 1]' -p proxy4nation
```

## ACTION `setreferral`

Set authorized referral

- Authority: `get_self()`

### params

- `{name} name` - referral account
- `{string} metadata` - referral metadata

### example

```bash
cleos push action proxy4nation setreferral '["tokenyieldio", "TokenYield.io - Track and Manage Blockchain Rewards"]' -p proxy4nation
```

## TABLE `rewards`

- `{symbol} symbol` - reward token symbol
- `{name} contract` - reward token contract
- `{int64_t} multiplier` - reward multiplier

### example

```json
{
  "symbol": "4,EOS",
  "contract": "eosio.token",
  "multiplier": 1
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

## TABLE `last`

- `{name} voter` - last voter to claim rewards
- `{asset} amount` - last claimed reward amount
- `{int64_t} rate` - APR rate pips 1/100 of 1%
- `{int64_t} interval` - claim interval in seconds
- `{time_point_sec} timestamp` - timestamp of last claim
- `{checksum256} trx_id` - transaction id

### example

```json
{
  "voter": "myaccount",
  "amount": "1.5000 EOS",
  "rate": 350,
  "interval": 86400,
  "timestamp": "2019-09-23T00:00:00",
  "trx_id": "0e90ad6152b9ba35500703bc9db858f6e1a550b5e1a8de05572f81cdcaae3a08"
}
```

## TABLE `referrals`

- `{name} name` - referral account
- `{string} metadata` - referral metadata

### example

```json
{
  "name": "tokenyieldio",
  "metadata": "TokenYield.io - Track and Manage Blockchain Rewards"
}
```

## TABLE `settings`

- `{int64_t} [rate=350]` - APR rate pips 1/100 of 1%
- `{int64_t} [interval=86400]` - claim interval in seconds
- `{int64_t} [rex=16]` - REX APR rate pips 1/100 of 1%

### example

```json
{
  "rate": 350,
  "interval": 86400,
  "rex": 16
}
```
