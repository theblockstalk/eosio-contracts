# EOSIO Blockchain Governance Templates

This repositories contains examples to show different blockchain governance configurations, as defined in their system contracts.

Glossary:
- BP: Block producer
- EOSIO: Blockchain software framework for deploying blockchains
- EOS: One of the many blockchains deployed using EOSIO. EOS uses just one of the many configurations for governance, discussed in this repository.

## Advantages

An EOSIO blockchain has a flexible, upgradeable architecture where most of the rules are defined through the system contracts. This makes EOSIO blockchains:

- have transparent rules of engagement
- have transparent operation of the rules of engagement
- easy configurable through smart contract with lots of official and community tooling
- maintain full compatibility with other EOSIO blockchains as core node parameters and block consensus does not need to change
- synchronize changes to the rules of engagement instantly accross the network (hard forks are much less needed, reducing administration and costs of governance upgrades)

## Governance Scope 

Most of the practical rules of an EOSIO blockchain are defined in it's system contracts.

The "eosio" contract defines the following rules:

- Consensus
- Account allocation and names
- Permissions and key management
- Resource allocation (CPU, NET and RAM)
- Block producer rewards and funding management
- Contract deploy/upgrading

A number of auxilirary contracts provide additional rules and features:

- ["eosio.token"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.token) a system token to use for resource management and payments
- ["eosio.msig"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.msig) asynchronous transaction authorization
- ["eosio.wrap"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.wrap) authorization bypass for the privledged accounts ("eosio") for blockchain management

Other governance contracts can be built to support the system contracts such as governance forums or decentralize funding allocation systems.

## The templates

These templates are adaptions of the ["eosio.bios"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.bios) contract - are bare minimum contract for governance with no special features.

Different governance components are provided as separate templates. e.g. consensus templates are separated from resource management templates. This is so that the different modules' functionality can be easily understood. A deployed EOSIO blockchain will need to blend all governance features into one set of contracts.

**Purpose and use**

These templates are (currently) **to understand the capacity for customization and to provide reference implementation guide only**. They have not been significantly tested, audited, or used in production.

**Production goverance contracts**

The following is a list of governance contracts in use on deployed blockchains:
- [EOS](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.system)
- [Telos](https://github.com/telosnetwork/telos.contracts/tree/master/contracts/eosio.system)
- [Worbi](https://github.com/worbli/worbli.contracts/tree/master/contracts/eosio.system)

## Contributions
If you would like to contribute to these templates please create a PR and it will be reviewed. To contract me please go to [jackandtheblockstalk.com](https://jackandtheblockstalk.com)

### Consensus

Different BP consensus governance can be built. Note that the core EOSIO consensus used is [aBFT](https://developers.eos.io/welcome/latest/protocol/consensus_protocol) and is governance agnostic. aBFT allows the system contract to update the schedule of block producers by calling the [set_proposed_producers](https://developers.eos.io/manuals/eosio.cdt/latest/group__privileged/#function-set_proposed_producers) intrinsic.

No matter which consensus governance is used, all EOSIO chains have block production with:

- 0.5s block time
- capacity for approximately 10,000-20,000 tps depending on usage, using EOSIO v2.0
- up to 125 BPs in the schedule, as specified by `max_producers` in [config.hpp](https://github.com/EOSIO/eos/blob/master/libraries/chain/include/eosio/chain/config.hpp#L106)

### Democratic
People can vote for producers. Votes are weighted as 1 person 1 vote. Each person can vote for 1 producer and can change at any time. The top 10 producers by votes enter the consensus schedule.

Note:
- Solution assumes that each person can only obtain one "people" account. This part of a democratic blockchain solution is not provided here.

**TODO**

### Delegated Proof of Stace DPOS
Token holders vote with for producers. Votes are weighted by token balance. Each account can vote for 1 producer and can change at any time. The top 66 producers by votes enter the consensus schedule.

`ACTION regproducer( const name& producer, const eosio::public_key& producer_key );`

`ACTION voteproducer( const name& voter_name, const name& producer );`

`ACTION onblock( ignore<block_header> header );`

### Proof of Authority POA

Accounts defined in the "active" permission of "eosio" account produces blocks and can update the permission, removing or adding new accounts up to 125.

Note this is the ["eosio.bios"](https://github.com/EOSIO/eosio.contracts/tree/master/contracts/eosio.bios) contract with no changes.

`ACTION setprods( const std::vector<eosio::producer_authority>& schedule )`

### Proof of Stace POS
The wealthies 100 block producers enter the consensus schedule. Wealth is determined by locked tokens which cannot be unlocked for 1 day. If proof is provided that a block producer produces two blocks for the same block production slot, then their locked token balance is removed.

**TODO**
