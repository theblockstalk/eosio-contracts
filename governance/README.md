# EOSIO Blockchain Governance Templates

This repositories contains examples to show different blockchain governance configurations, as defined in their system contracts.

Background about the EOSIO software features and customizability:
[https://medium.com/coinmonks/difference-between-eosio-software-and-eos-blockchain-13bcc57d1d9d](https://medium.com/coinmonks/difference-between-eosio-software-and-eos-blockchain-13bcc57d1d9d)

**Purpose and use**

These templates are (currently) **to understand the capacity for customization and to provide a reference implementation guide only**. They have not been significantly tested, audited, or used in production.

These templates are adaptions of the ["eosio.bios"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.bios) contract - are bare minimum contract for governance with no special features.

Different governance components are provided as separate templates. e.g. consensus templates are separated from resource management templates. This is so that the different modules' functionality can be easily understood. A deployed EOSIO blockchain will need to blend all governance features into one set of contracts.

** Contributions **
If you would like to contribute to these templates please create a PR and it will be reviewed. To contract me please go to [jackandtheblockstalk.com](https://jackandtheblockstalk.com)

## Consensus Templates

Different BP consensus governance can be built. Note that the core EOSIO consensus used is [aBFT](https://developers.eos.io/welcome/latest/protocol/consensus_protocol) and is governance agnostic. aBFT allows the system contract to update the schedule of block producers by calling the [set_proposed_producers](https://developers.eos.io/manuals/eosio.cdt/latest/group__privileged/#function-set_proposed_producers) intrinsic.

No matter which consensus governance is used, all EOSIO chains have block production with:

- 0.5s block time
- capacity for approximately 10,000-20,000 tps depending on usage, using EOSIO v2.0
- up to 125 BPs in the schedule, as specified by `max_producers` in [config.hpp](https://github.com/EOSIO/eos/blob/master/libraries/chain/include/eosio/chain/config.hpp#L106)

### [Democratic](https://github.com/theblockstalk/eosio-contracts/tree/master/governance/Democratic)
People can vote for producers. Votes are weighted as 1 person 1 vote. Each person can vote for 1 producer and can change at any time. The top 10 producers by votes enter the consensus schedule.

Note:
- Solution assumes that each person can only obtain one "people" account. This part of a democratic blockchain solution is not provided here.

`ACTION newaccount( name creator, name name, uint32_t account_type, authority owner, authority active)`

`ACTION newperson( name creator, name name, uint32_t account_type, authority owner, authority active)`

`ACTION newentity( name creator, name name, uint32_t account_type, authority owner, authority active)`

### [Delegated Proof of Stace DPOS](https://github.com/theblockstalk/eosio-contracts/tree/master/governance/DPOS])
Token holders vote with for producers. Votes are weighted by token balance. Each account can vote for 1 producer and can change at any time. The top 66 producers by votes enter the consensus schedule.

`ACTION regproducer( const name& producer, const eosio::public_key& producer_key );`

`ACTION voteproducer( const name& voter_name, const name& producer );`

`ACTION onblock( ignore<block_header> header );`

### [Proof of Authority POA](https://github.com/theblockstalk/eosio-contracts/tree/master/governance/POA)

Accounts defined in the "active" permission of "eosio" account produces blocks and can update the permission, removing or adding new accounts up to 125.

Note this is the ["eosio.bios"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.bios) contract with no changes.

`ACTION setprods( const std::vector<eosio::producer_authority>& schedule )`

### [Proof of Stace POS](https://github.com/theblockstalk/eosio-contracts/tree/master/governance/POS)
The wealthies 100 block producers enter the consensus schedule. Wealth is determined by locked tokens which cannot be unlocked for 1 day. If proof is provided that a block producer produces two blocks for the same block production slot, then their locked token balance is removed.

`ACTION regproducer( const name& producer, const eosio::public_key& producer_key );`

`ACTION onblock( ignore<block_header> header );`

`[eosio::on_notify("eosio.token::transfer")]] void stake(name from, name to, assert quantity, std::string memo);`

`ACTION unstake( name producer, assert quantity, std::string memo );`

`ACTION punishprod( name punisher, name producer_to_punish /* TODO */);`
