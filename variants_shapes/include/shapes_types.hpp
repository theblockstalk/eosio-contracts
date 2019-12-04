#ifndef shapes_types_h
#define shapes_types_h
#include <string>

struct Shape {
  std::string shape_name;
};

struct Rectangle : public Shape {
  uint32_t width;
  uint32_t height;
};

struct Circle : public Shape {
  uint32_t diameter;
};

#endif