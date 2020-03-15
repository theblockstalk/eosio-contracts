#include <eosio.bios.hpp>

namespace eosiobios {

void bios::register_producer( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location ) {
   auto prod = _producers.find( producer.value );
   const auto ct = current_time_point();

   eosio::public_key producer_key{};

   std::visit( [&](auto&& auth ) {
      if( auth.keys.size() == 1 ) {
         // if the producer_authority consists of a single key, use that key in the legacy producer_key field
         producer_key = auth.keys[0].key;
      }
   }, producer_authority );

   if ( prod != _producers.end() ) {
      _producers.modify( prod, producer, [&]( producer_info& info ){
         info.producer_key       = producer_key;
         info.is_active          = true;
         info.url                = url;
         info.location           = location;
         info.producer_authority.emplace( producer_authority );
         if ( info.last_claim_time == time_point() )
            info.last_claim_time = ct;
      });

      auto prod2 = _producers2.find( producer.value );
      if ( prod2 == _producers2.end() ) {
         _producers2.emplace( producer, [&]( producer_info2& info ){
            info.owner                     = producer;
            info.last_votepay_share_update = ct;
         });
         update_total_votepay_share( ct, 0.0, prod->total_votes );
         // When introducing the producer2 table row for the first time, the producer's votes must also be accounted for in the global total_producer_votepay_share at the same time.
      }
   } else {
      _producers.emplace( producer, [&]( producer_info& info ){
         info.owner              = producer;
         info.total_votes        = 0;
         info.producer_key       = producer_key;
         info.is_active          = true;
         info.url                = url;
         info.location           = location;
         info.last_claim_time    = ct;
         info.producer_authority.emplace( producer_authority );
      });
      _producers2.emplace( producer, [&]( producer_info2& info ){
         info.owner                     = producer;
         info.last_votepay_share_update = ct;
      });
   }
}

void bios::regproducer( const name& producer, const eosio::public_key& producer_key, const std::string& url, uint16_t location ) {
   require_auth( producer );
   check( url.size() < 512, "url too long" );

   register_producer( producer, convert_to_block_signing_authority( producer_key ), url, location );
}

void bios::regproducer2( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location ) {
   require_auth( producer );
   check( url.size() < 512, "url too long" );

   std::visit( [&](auto&& auth ) {
      check( auth.is_valid(), "invalid producer authority" );
   }, producer_authority );

   register_producer( producer, producer_authority, url, location );
}

void bios::unregprod( const name& producer ) {
   require_auth( producer );

   const auto& prod = _producers.get( producer.value, "producer not found" );
   _producers.modify( prod, same_payer, [&]( producer_info& info ){
      info.deactivate();
   });
}

void system_contract::voteproducer( const name& voter_name, const name& producer ) {
   require_auth( voter_name );

   auto voter = _voters.find( voter_name.value );
   if( voter != _voters.end() ) {

   } else {

   }


   // vote_stake_updater( voter_name );
   // update_votes( voter_name, producer );
}

void system_contract::update_votes( const name& voter_name, const name& producer) {
      auto voter = _voters.find( voter_name.value );
      /**
       * The first time someone votes we calculate and set last_vote_weight. Since they cannot unstake until
       * after the chain has been activated, we can use last_vote_weight to determine that this is
       * their first vote and should consider their stake activated.
       */
      if( _gstate.thresh_activated_stake_time == time_point() && voter->last_vote_weight <= 0.0 ) {
         _gstate.total_activated_stake += voter->staked;
         if( _gstate.total_activated_stake >= min_activated_stake ) {
            _gstate.thresh_activated_stake_time = current_time_point();
         }
      }

      auto new_vote_weight = stake2vote( voter->staked );
      if( voter->is_proxy ) {
         new_vote_weight += voter->proxied_vote_weight;
      }

      std::map<name, std::pair<double, bool /*new*/> > producer_deltas;
      if ( voter->last_vote_weight > 0 ) {
         if( voter->proxy ) {
            auto old_proxy = _voters.find( voter->proxy.value );
            check( old_proxy != _voters.end(), "old proxy not found" ); //data corruption
            _voters.modify( old_proxy, same_payer, [&]( auto& vp ) {
                  vp.proxied_vote_weight -= voter->last_vote_weight;
               });
            propagate_weight_change( *old_proxy );
         } else {
            for( const auto& p : voter->producers ) {
               auto& d = producer_deltas[p];
               d.first -= voter->last_vote_weight;
               d.second = false;
            }
         }
      }

      if( proxy ) {
         auto new_proxy = _voters.find( proxy.value );
         check( new_proxy != _voters.end(), "invalid proxy specified" ); //if ( !voting ) { data corruption } else { wrong vote }
         check( !voting || new_proxy->is_proxy, "proxy not found" );
         if ( new_vote_weight >= 0 ) {
            _voters.modify( new_proxy, same_payer, [&]( auto& vp ) {
                  vp.proxied_vote_weight += new_vote_weight;
               });
            propagate_weight_change( *new_proxy );
         }
      } else {
         if( new_vote_weight >= 0 ) {
            for( const auto& p : producers ) {
               auto& d = producer_deltas[p];
               d.first += new_vote_weight;
               d.second = true;
            }
         }
      }

      const auto ct = current_time_point();
      double delta_change_rate         = 0.0;
      double total_inactive_vpay_share = 0.0;
      for( const auto& pd : producer_deltas ) {
         auto pitr = _producers.find( pd.first.value );
         if( pitr != _producers.end() ) {
            if( voting && !pitr->active() && pd.second.second /* from new set */ ) {
               check( false, ( "producer " + pitr->owner.to_string() + " is not currently registered" ).data() );
            }
            double init_total_votes = pitr->total_votes;
            _producers.modify( pitr, same_payer, [&]( auto& p ) {
               p.total_votes += pd.second.first;
               if ( p.total_votes < 0 ) { // floating point arithmetics can give small negative numbers
                  p.total_votes = 0;
               }
               _gstate.total_producer_vote_weight += pd.second.first;
               //check( p.total_votes >= 0, "something bad happened" );
            });
            auto prod2 = _producers2.find( pd.first.value );
            if( prod2 != _producers2.end() ) {
               const auto last_claim_plus_3days = pitr->last_claim_time + microseconds(3 * useconds_per_day);
               bool crossed_threshold       = (last_claim_plus_3days <= ct);
               bool updated_after_threshold = (last_claim_plus_3days <= prod2->last_votepay_share_update);
               // Note: updated_after_threshold implies cross_threshold

               double new_votepay_share = update_producer_votepay_share( prod2,
                                             ct,
                                             updated_after_threshold ? 0.0 : init_total_votes,
                                             crossed_threshold && !updated_after_threshold // only reset votepay_share once after threshold
                                          );

               if( !crossed_threshold ) {
                  delta_change_rate += pd.second.first;
               } else if( !updated_after_threshold ) {
                  total_inactive_vpay_share += new_votepay_share;
                  delta_change_rate -= init_total_votes;
               }
            }
         } else {
            if( pd.second.second ) {
               check( false, ( "producer " + pd.first.to_string() + " is not registered" ).data() );
            }
         }
      }

      update_total_votepay_share( ct, -total_inactive_vpay_share, delta_change_rate );

      _voters.modify( voter, same_payer, [&]( auto& av ) {
         av.last_vote_weight = new_vote_weight;
         av.producers = producers;
         av.proxy     = proxy;
      });
   }

void bios::onblock( ignore<block_header> ) {
   require_auth(get_self());

   block_timestamp timestamp;
   name producer;
   _ds >> timestamp >> producer;

   if( timestamp.slot - _gstate.last_producer_schedule_update.slot > 120 ) {
      update_elected_producers( timestamp );
   }
}

void bios::update_elected_producers( const block_timestamp& block_time ) {
   auto idx = _producers.get_index<"prototalvote"_n>();

   std::vector< <eosio::producer_authority > top_producers;
   top_producers.reserve(NUMBER_PRODUCERS);

   for( auto it = idx.cbegin(); it != idx.cend() && top_producers.size() < NUMBER_PRODUCERS && 0 < it->total_votes && it->active(); ++it ) {
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

   std::sort( top_producers.begin(), top_producers.end(), []( const value_type& lhs, const value_type& rhs ) {
      return lhs.first.producer_name < rhs.first.producer_name;
   } );

   std::vector<eosio::producer_authority> producers;

   producers.reserve(top_producers.size());
   for( auto& item : top_producers )
      producers.push_back( std::move(item.first) );

   set_proposed_producers( producers );
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

void bios::setprods( const std::vector<eosio::producer_authority>& schedule ) {
   require_auth( get_self() );
   set_proposed_producers( schedule );
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