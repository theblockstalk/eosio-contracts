#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

CONTRACT openfood : public contract {
  public:
    using contract::contract;

    ACTION addversion(name from, uint64_t version_id, string abi);
    ACTION createsoy(name from, name to, checksum256 hash, uint64_t version_id);
    ACTION sendsoy(name from, name to, checksum256 prev_hash, checksum256 hash, uint64_t version_id);
    
  private:
    TABLE proof {
      name    from;
      name    to;
      checksum256 hash;
      uint64_t version_id;
      uint64_t primary_key() const { return from.value; }
      checksum256 hash_index() const { return hash; }
    };
    
    typedef multi_index<"proofs"_n, proof,
    indexed_by<"byhash"_n, const_mem_fun<proof, checksum256, &proof::hash_index>>
      > proofs_table;

    TABLE version {
      uint64_t version_id;
      name updated_added_by;
      time_point updated;
      string abi;
      uint64_t primary_key() const { return version_id; }
    };
    typedef multi_index<name("versions"), version> versions_table;

    void checkVersionExists(uint64_t version_id);
};