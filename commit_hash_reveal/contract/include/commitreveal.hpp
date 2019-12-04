#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>

using namespace std;
using namespace eosio;

CONTRACT commitreveal : public contract {
  public:
    using contract::contract;

    ACTION commit(name from, const checksum256 &message_hash);
    ACTION reveal(name from, string message);
    ACTION clear();

  private:
    TABLE messages {
      name    user;
      checksum256 hash;
      time_point last_updated;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
