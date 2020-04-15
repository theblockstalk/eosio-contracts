#include <eosio.bios.hpp>

namespace eosiobios {

bios::bios(name receiver, name code, datastream<const char*> ds ) :
   contract(receiver, code, ds),
   _people(get_self(), get_self().value),
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
         info.total_votes        = 0;
         info.producer_key       = producer_pub_key;
         info.producer_authority.emplace( producer_authority );
      });
   }
}

void bios::voteproducer( const name& voter_name, const name& producer ) {
   require_auth( voter_name );

   auto person = _people.get( voter_name.value, "Only people can vote"); // Will throw error if no person found
   name previous_producer = person.producer;
   check(person.voted && previous_producer == producer, "You are already voting for this producer");
   
   _people.modify( person, voter_name, [&]( auto& p ) {
      p.producer = producer;
      if (!person.voted)
         p.voted = true;
   });

   auto pitr = _producers.get( producer.value, "Producer has not registered");

   _producers.modify( pitr, same_payer, [&]( auto& p ) {
      p.total_votes += 1;
   });

   auto pitr2 = _producers.find( previous_producer.value );
   _producers.modify( pitr2, same_payer, [&]( auto& p ) {
      p.total_votes -= 1;
   });
}

void bios::onblock( ignore<block_header> ) {
   require_auth(get_self());

   block_timestamp timestamp;
   name producer;
   _ds >> timestamp >> producer;

   auto gstate = _gstate.get();
   
   if( timestamp.slot - gstate.last_producer_schedule_update.slot > 120 ) {
      auto idx = _producers.get_index<"prototalvote"_n>();

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

void bios::newaccount( name creator, name name, ignore<authority> owner, ignore<authority> active) {
   // This action can only be called by inline action from "eosio"
   check(eosio::get_sender() == get_self(), "Cannot call newaccount directly");
}

void bios::newperson( name creator, name name, authority owner, authority active) {
   // Check that this person has not already created an accounts...

   _people.emplace(name, [&]( auto& p ) {
      p.owner = name;
   });

   bios::newaccount_action(get_self(), {creator, "active"_n})
      .send(creator, name, owner, active);
}

void bios::newentity( name creator, name name, authority owner, authority active) {
   bios::newaccount_action(get_self(), {creator, "active"_n})
      .send(creator, name, owner, active);
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