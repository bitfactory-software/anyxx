#pragma once

#include <virtual_void/meta/class.hpp>

namespace virtual_void::open_method {

template <typename METHOD_BASE>
struct domain {
  meta::classes_with_bases classes;
  std::vector<METHOD_BASE*> open_methods;
};

}  // namespace virtual_void::open_method