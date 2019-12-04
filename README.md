# EOSIO smart contract examples

### polymorphism_shapes

How NOT to create polymorphic structures in the EOSIO database (see variants_shapes for a correct solution)

### variants_shapes

Using variants to store different shapes in the EOSIO database

### variants_uints

A simple version of variants_shapes that stores uint8_t, uint16_t or a string in a table

### commit_hash_reveal

A full blockchain application:
- EOSIO smart contract with commit/reveal hash pattern
- A no-framework (no React, Vue etc) front-end web app to connect to the smart contract.
- Unit test suit using eoslime.