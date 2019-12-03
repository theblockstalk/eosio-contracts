#include <shapesvar.hpp>

ACTION shapesvar::newrectangle(uint32_t _width, uint32_t _height) {
  name self = get_self();
  shapes_table _shapes(self, self.value);

  _shapes.emplace(self, [&](auto& row) {
    row.id = _shapes.available_primary_key();

    shape_variant r = Rectangle(_width, _height);
    row.shape = r;
  });
}

ACTION shapesvar::clear() {
  require_auth(get_self());

  shapes_table _shapes(get_self(), get_self().value);

  // Delete all records in table
  auto row_itr = _shapes.begin();
  while (row_itr != _shapes.end()) {
    row_itr = _shapes.erase(row_itr);
  }
}