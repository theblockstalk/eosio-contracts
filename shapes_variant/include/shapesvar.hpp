#include <eosio/eosio.hpp>
#include "../shapes.cpp"
#include <variant>

using namespace std;
using namespace eosio;

typedef variant<Rectangle, Circle> shape_variant;

CONTRACT shapesvar : public contract {
  public:
    using contract::contract;

    ACTION newrectangle(uint32_t _width, uint32_t _height);
    ACTION clear();

  private:
    TABLE shapes_struct {
      uint64_t id;
      shape_variant shape;
      auto primary_key() const { return id; }
    };
    typedef multi_index<name("shapes"), shapes_struct> shapes_table;
};
