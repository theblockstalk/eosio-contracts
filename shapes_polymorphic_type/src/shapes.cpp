#include <shapes.hpp>

ACTION shapes::newrectangle(uint32_t _width, uint32_t _height) {
  name self = get_self();
  shapes_table _shapes(self, self.value);

  _shapes.emplace(self, [&](auto& row) {
    row.id = _shapes.available_primary_key();

    Rectangle s = Rectangle(_width, _height);
    row.shape = s;
  });
}

ACTION shapes::clear() {
  require_auth(get_self());

  shapes_table _shapes(get_self(), get_self().value);

  auto row_itr = _shapes.begin();
  while (row_itr != _shapes.end()) {
    row_itr = _shapes.erase(row_itr);
  }
}