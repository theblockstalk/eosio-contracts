

Centralized
"eosio" account centrally decides who will produce blocks (max 125 in schedule)
`ACTION setprods( const std::vector<eosio::producer_authority>& schedule )`

Democratic

DPoS (1 token 1 vote)
`ACTION regproducer( const name& producer, const eosio::public_key& producer_key );`
`ACTION voteproducer( const name& voter_name, const name& producer );`
`ACTION onblock( ignore<block_header> header );`

DPoS (EOS)

DPoS (Telos)

POA

POS

## Not implemented that may need to be considered

* Producer payment
* Sytem token initialization and integration with resource management accounting
* Action wrappers

## Other contracts to consider

* Authorization bypass eosio.warp
* Asyncronous transaction authorization eosio.msig
* System token eosio.token

## Kept with default eosio.bios implmentation

* Resource (RAM, CPU and NET) allocation
* Account allocation
* Account permission management
* Account names
* Contract deploy/upgrade management