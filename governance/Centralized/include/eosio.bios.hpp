#pragma once

#include <eosio/action.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/fixed_bytes.hpp>
#include <eosio/privileged.hpp>
#include <eosio/producer_schedule.hpp>

namespace eosiobios {

   using eosio::action_wrapper;
   using eosio::check;
   using eosio::checksum256;
   using eosio::ignore;
   using eosio::name;
   using eosio::permission_level;
   using eosio::public_key;

   struct permission_level_weight {
      permission_level  permission;
      uint16_t          weight;
   };

   struct key_weight {
      eosio::public_key  key;
      uint16_t           weight;
   };

   struct wait_weight {
      uint32_t           wait_sec;
      uint16_t           weight;
   };

   struct authority {
      uint32_t                              threshold = 0;
      std::vector<key_weight>               keys;
      std::vector<permission_level_weight>  accounts;
      std::vector<wait_weight>              waits;
   };

   struct block_header {
      uint32_t                                  timestamp;
      name                                      producer;
      uint16_t                                  confirmed = 0;
      checksum256                               previous;
      checksum256                               transaction_mroot;
      checksum256                               action_mroot;
      uint32_t                                  schedule_version = 0;
      std::optional<eosio::producer_schedule>   new_producers;
   };

   class [[eosio::contract("eosio.bios")]] bios : public eosio::contract {
      public:
         using contract::contract;

         ACTION newaccount( name             creator,
                          name             name,
                          ignore<authority> owner,
                          ignore<authority> active){}
                          
         ACTION updateauth(  ignore<name>  account,
                           ignore<name>  permission,
                           ignore<name>  parent,
                           ignore<authority> auth ) {}

         ACTION deleteauth( ignore<name>  account,
                          ignore<name>  permission ) {}

         ACTION linkauth(  ignore<name>    account,
                         ignore<name>    code,
                         ignore<name>    type,
                         ignore<name>    requirement  ) {}

         ACTION unlinkauth( ignore<name>  account,
                          ignore<name>  code,
                          ignore<name>  type ) {}

         ACTION canceldelay( ignore<permission_level> canceling_auth, ignore<checksum256> trx_id ) {}

         ACTION setcode( name account, uint8_t vmtype, uint8_t vmversion, const std::vector<char>& code ) {}

         ACTION setabi( name account, const std::vector<char>& abi );

         ACTION onerror( ignore<uint128_t> sender_id, ignore<std::vector<char>> sent_trx );

         ACTION setpriv( name account, uint8_t is_priv );
         
         ACTION setalimits( name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight );

         ACTION setprods( const std::vector<eosio::producer_authority>& schedule );

         ACTION setparams( const eosio::blockchain_parameters& params );

         ACTION reqauth( name from );

         ACTION activate( const eosio::checksum256& feature_digest );

         ACTION reqactivated( const eosio::checksum256& feature_digest );

         TABLE abi_hash {
            name              owner;
            checksum256       hash;
            uint64_t primary_key()const { return owner.value; }

            EOSLIB_SERIALIZE( abi_hash, (owner)(hash) )
         };

         typedef eosio::multi_index< "abihash"_n, abi_hash > abi_hash_table;
   };
}
