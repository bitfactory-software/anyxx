#pragma once

#include "../../class_hierarchy/class_hierarchy.h"
#include "table.h"

namespace virtual_void::open_method {

template <typename METHOD_BASE>
struct domain {
  class_hierarchy::classes_with_bases classes;
  std::vector<METHOD_BASE*> open_methods;
};

}  // namespace virtual_void::open_method