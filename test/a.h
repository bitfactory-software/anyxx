#include <string>

import virtual_void;
//#include "virtual_void/virtual_void.h"
#include "virtual_void/utillities/VIRTUAL_DESTRUCTOR_FOR_DEBUGGING.h"

struct A {
  A(const std::string sp) : s(sp) {}
  std::string s;
  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING ~A() = default;
};
