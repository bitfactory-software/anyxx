#pragma once

#include <virtual_void/class_hierarchy/class_hierarchy.hpp>

namespace virtual_void::open_method {

template <typename METHOD_BASE>
struct domain {
  class_hierarchy::classes_with_bases classes;
  std::vector<METHOD_BASE*> open_methods;
};

template <typename RET, typename... ARGS>
struct translate_erased_function;

template <typename RET, is_constness CONSTNESS, typename... OTHER_ARGS>
struct translate_erased_function<RET, CONSTNESS, OTHER_ARGS...> {
  using type = RET (*)(void_t<CONSTNESS>, OTHER_ARGS...);
};

}  // namespace virtual_void::open_method