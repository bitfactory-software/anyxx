#pragma once

#include "../class_hierarchy/class_hierarchy.h"
#include "table.h"

namespace virtual_void::open_method {

template <typename METHOD_BASE>
struct domain {
  class_hierarchy::classes_with_bases classes;
  std::vector<METHOD_BASE*> open_methods;
};

template <typename RET, typename... ARGS>
struct translate_erased_function;

template <typename RET, is_const_specifier CONST_SPECIFIER, typename... OTHER_ARGS>
struct translate_erased_function<RET, CONST_SPECIFIER, OTHER_ARGS...> {
  using type = RET (*)(void_t<CONST_SPECIFIER>, OTHER_ARGS...);
};

}  // namespace virtual_void::open_method