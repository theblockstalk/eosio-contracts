#include <eosio.bios.hpp>

namespace eosiobios {

bios::bios(name receiver, name code, datastream<const char*> ds ) :
   contract(receiver, code, ds),
   _producers(get_self(), get_self().value),
   _gstate(get_self(), get_self().value) {

   block_timestamp timestamp;
   _ds >> timestamp;
   _gstate.get_or_create(get_self(), {.last_producer_schedule_update=timestamp});
}

void bios::regproducer( const name& producer, const eosio::public_key& producer_key ) {
   require_auth( producer );

   const eosio::block_signing_authority producer_authority = bios::convert_to_block_signing_authority( producer_key );
   auto prod = _producers.find( producer.value );

   eosio::public_key producer_pub_key{};

   std::visit( [&](auto&& auth ) {
      if( auth.keys.size() == 1 ) {
         producer_pub_key = auth.keys[0].key;
      }
   }, producer_authority );

   if ( prod != _producers.end() ) {
      _producers.modify( prod, producer, [&]( producer_info& info ){
         info.producer_key       = producer_pub_key;
         info.producer_authority.emplace( producer_authority );
      });
   } else {
      _producers.emplace( producer, [&]( producer_info& info ){
         info.owner              = producer;
         info.producer_key       = producer_pub_key;
         info.producer_authority.emplace( producer_authority );
      });
   }
}

void bios::onblock( ignore<block_header> ) {
   require_auth(get_self());

   block_timestamp timestamp;
   name producer;
   _ds >> timestamp >> producer;

   auto gstate = _gstate.get();
   
   if( timestamp.slot - gstate.last_producer_schedule_update.slot > 120 ) {
      auto idx = _producers.get_index<"stakeamount"_n>();

      std::vector< eosio::producer_authority > top_producers;
      top_producers.reserve(NUMBER_PRODUCERS);

      for( auto it = idx.cbegin(); it != idx.cend() && top_producers.size() < NUMBER_PRODUCERS && 0 < it->total_votes; ++it ) {
         top_producers.emplace_back(
            eosio::producer_authority{
               .producer_name = it->owner,
               .authority     = it->get_producer_authority()
            }
         );
      }

      if( top_producers.size() == 0 ) {
         return;
      }

      std::sort( top_producers.begin(), top_producers.end(), []( const eosio::producer_authority& lhs, const eosio::producer_authority& rhs ) {
         return lhs.producer_name < rhs.producer_name;
      } );

      std::vector<eosio::producer_authority> producers;

      producers.reserve(top_producers.size());
      for( auto& item : top_producers )
         producers.push_back( std::move(item) );

      _gstate.set({.last_producer_schedule_update=timestamp}, get_self());
      set_proposed_producers( producers );
   }
}

void bios::stake(name from, name to, assert quantity, std::string memo) {
   check(quantity.symbol = symbol("SYS"), "Can only state the system token");

   auto prod = _producers.get( producer.value , "Producer not found");

   _producers.modify( prod, eosio::same_payer, [&]( producer_info& info ){
      info.total_stake += quantity.amount;
      info.stake_time = current_time_point();
   });
}

void bios::unstake( name producer, assert quantity, std::string memo ) {
   check(quantity.symbol = symbol("SYS"), "Can only unstate the system token");

   auto prod = _producers.get( producer.value , "Producer not found");

   time_point now = current_time_point();
   check( now - prod->stake_time.time_since_epoch() > 60*60*24, "Can only unstake 1 day after last stake or unstake action");

   _producers.modify( prod, eosio::same_payer, [&]( producer_info& info ){
      info.total_stake -= quantity.amount;
      info.stake_time = current_time_point;
   });
}

void bios::punishprod( name punisher, name producer_to_punish /* TODO */) {
   // TODO
   // 1. check that block headers are valid
   // 2. check block headers have different hash
   // 3. check block headers have same slot (block number)
   // 4. check both blocks were authorized by the same producer (may need to use oracle to prove signature from blockchain history of keys)
   // 5. Remove all staked balance from producer table
}

void bios::setabi( name account, const std::vector<char>& abi ) {
   abi_hash_table table(get_self(), get_self().value);
   auto itr = table.find( account.value );
   if( itr == table.end() ) {
      table.emplace( account, [&]( auto& row ) {
         row.owner = account;
         row.hash  = eosio::sha256(const_cast<char*>(abi.data()), abi.size());
      });
   } else {
      table.modify( itr, eosio::same_payer, [&]( auto& row ) {
         row.hash = eosio::sha256(const_cast<char*>(abi.data()), abi.size());
      });
   }
}

void bios::onerror( ignore<uint128_t>, ignore<std::vector<char>> ) {
   check( false, "the onerror action cannot be called directly" );
}

void bios::setpriv( name account, uint8_t is_priv ) {
   require_auth( get_self() );
   set_privileged( account, is_priv );
}

void bios::setalimits( name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
   require_auth( get_self() );
   set_resource_limits( account, ram_bytes, net_weight, cpu_weight );
}

void bios::setparams( const eosio::blockchain_parameters& params ) {
   require_auth( get_self() );
   set_blockchain_parameters( params );
}

void bios::reqauth( name from ) {
   require_auth( from );
}

void bios::activate( const eosio::checksum256& feature_digest ) {
   require_auth( get_self() );
   preactivate_feature( feature_digest );
}

void bios::reqactivated( const eosio::checksum256& feature_digest ) {
   check( is_feature_activated( feature_digest ), "protocol feature is not activated" );
}

}