# EOS Nation - Proxy Staking Service

## ACTION

- [`claim`](#action-claim)
- [`signup`](#action-signup)
- [`refresh`](#action-refresh)
- [`setrate`](#action-setrate)
- [`setinterval`](#action-setinterval)
- [`setreward`](#action-setreward)

## TABLE

- [`rewards`](#rewards-table)
- [`settings`](#settings-table)
- [`voters`](#voters-table)

## ACTION `signup`

Signup owner to EOS Nation Proxy Staking Service

- Authority: `owner`

### params

- `{name} owner` - owner account

### example

```bash
cleos push action proxy4nation signup '["myaccount"]' -p myaccount
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

## TABLE `rewards`

- `{asset} rewards` - culmative rewards of all claims
- `{name} contract` - reward token contract
- `{int64_t} multiplier` - reward multiplier
- `{asset} last_reward` - last claimed reward amount
- `{name} last_user` - last user to claim rewards
- `{time_point_sec} last_claim` - timestamp of last claim

### example

```json
{
  "rewards": "20.0000 EOS",
  "contract": "eosio.token",
  "multiplier": 1,
  "last_reward": "1.5000 EOS",
  "last_user": "myaccount",
  "last_claim": "2019-09-23T00:00:00"
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

### example

```json
{
  "owner": "myaccount",
  "proxy": "proxy4nation",
  "producers": [],
  "staked": 6000,
  "last_vote_weight": "5361455468.19293689727783203",
  "is_proxy": 0,
  "next_claim_period": "2019-08-07T18:37:37"
}
```

## TABLE `settings`

- `{int64_t} [rate=250]` - APR rate pips 1/100 of 1%
- `{int64_t} [interval=86400]` - claim interval in seconds

### example

```json
{
  "rate": 250,
  "interval": 86400
}
```
