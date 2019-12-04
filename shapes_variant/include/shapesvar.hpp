#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

struct Shape {
  string shape_name;
};

struct Rectangle : public Shape {
  uint32_t width;
  uint32_t height;
};

struct Circle : public Shape {
  uint32_t diameter;
};

CONTRACT shapesvar : public contract {
  public:
    using contract::contract;

    ACTION newrectangle(uint32_t _width, uint32_t _height);
    ACTION newcircle(uint32_t _diameter);
    ACTION printme();
    ACTION clear();

  private:
    TABLE shapes_struct {
      uint64_t id;
      std::variant<Rectangle, Circle> shape_var;
      auto primary_key() const { return id; }
      EOSLIB_SERIALIZE(shapes_struct, (id)(shape_var))
    };
    typedef multi_index<name("shapes"), shapes_struct> shapes_table;
};
