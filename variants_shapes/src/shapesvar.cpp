#include <shapesvar.hpp>

ACTION shapesvar::newrectangle(uint32_t _width, uint32_t _height) {
  name self = get_self();
  shapes_table _shapes(self, self.value);

  _shapes.emplace(self, [&](auto& row) {
    row.id = _shapes.available_primary_key();
    Rectangle r = {"rectangle", _width, _height};
    row.shape_var = r;
  });
}

ACTION shapesvar::newcircle(uint32_t _diameter) {
  name self = get_self();
  shapes_table _shapes(self, self.value);

  _shapes.emplace(self, [&](auto& row) {
    row.id = _shapes.available_primary_key();
    Circle c = {"circle", _diameter};
    row.shape_var = c;
  });
}

ACTION shapesvar::printme() {
  shapes_table _shapes(get_self(), get_self().value);

  auto row_itr = _shapes.begin();
  while (row_itr != _shapes.end()) {
    std::variant<Rectangle, Circle> shape_var = row_itr->shape_var;
    switch(shape_var.index())
    {
      case 0:
        {
          Rectangle r = std::get<Rectangle>(shape_var);
          print("Rectangle(width: ", r.width, ", height: ", r.height, ") ");
          break;
        }
      case 1:
        {
          Circle c = std::get<Circle>(shape_var);
          print("Circle(diameter: ", c.diameter, ") ");
          break;
        }
    }
    ++row_itr;
  }
}
    

ACTION shapesvar::clear() {
  require_auth(get_self());

  shapes_table _shapes(get_self(), get_self().value);

  auto row_itr = _shapes.begin();
  while (row_itr != _shapes.end()) {
    row_itr = _shapes.erase(row_itr);
  }
}
