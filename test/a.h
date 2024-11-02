#include <string>

struct A {
  A(const std::string sp) : s(sp) {}
  std::string s;
  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING ~A() = default;
};
