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
      /*
       * EOSLIB_SERIALIZE macro needed.
       * If not added then get_table_rows returns error:
       * {"code":500,"message":"Internal Service Error","error":{"code":3015013,"name":"unpack_exception","what":"Unpack data exception","details":[{"message":"Unpacked invalid tag (127) for variant 'variant_struct.var'","file":"abi_serializer.cpp","line_number":367,"method":"_binary_to_variant"}]}}
       * and printme() will show incorrect values
       * Row id: 0 val = 255Row id: 1 val = 255
       */
      EOSLIB_SERIALIZE(variant_struct, (id)(var)) // abi is the same with or without this...
    };
    typedef multi_index<name("variants"), variant_struct> variants_table;
};