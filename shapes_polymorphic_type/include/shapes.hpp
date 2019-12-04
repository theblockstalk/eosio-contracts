#include <eosio/eosio.hpp>
#include "shapes.hpp"

using namespace std;
using namespace eosio;

CONTRACT shapes : public contract {
  public:
    using contract::contract;

    ACTION newrectangle(uint32_t _width, uint32_t _height);
    ACTION clear();

  private:
    TABLE shapes_struct {
      uint64_t id;
      // The following will not work as the polymorphic type of Shape (it's subclass) will not be readable at runtime
      Shape shape;
      auto primary_key() const { return id; }
    };
    typedef multi_index<name("shapes"), shapes_struct> shapes_table;
};