#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT variantuint : public contract {
  public:
    using contract::contract;
    
    ACTION addvalues();
    ACTION printme();
    ACTION clear();

  private:
    TABLE variant_struct {
      uint64_t id;
      std::variant<uint8_t, uint16_t> var;
      auto primary_key() const { return id; }
      EOSLIB_SERIALIZE(variant_struct, (id)(var)) // abi is the same with or without this...
    };
    typedef multi_index<name("variants"), variant_struct> variants_table;
};
