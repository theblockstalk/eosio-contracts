

Centralized
"eosio" account centrally decides who will produce blocks (max 125 in schedule)
`setprods( const std::vector<eosio::producer_authority>& schedule )`

Democratic

DPoS (simplified from EOS)
1 token 1 vote

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