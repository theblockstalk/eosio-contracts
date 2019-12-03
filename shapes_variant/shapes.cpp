#ifndef sovreign_h
#define sovereign_h
#include <string>

using std::string;

struct Shape {
  string shape_name;

  Shape(string _name) : shape_name(_name) {}
};

struct Rectangle : public Shape {
  uint32_t width;
  uint32_t height;

  Rectangle() : Shape("rectangle"), width(0), height(0) {}
  Rectangle(uint32_t _width, uint32_t _height) : Shape("rectangle"), width(_width), height(_height) {}
};

struct Circle : public Shape {
  uint32_t diameter;

  Circle() : Shape("circle"), diameter(0) {}
  Circle(uint32_t _diameter) : Shape("circle"), diameter(_diameter) {}
};

#endif