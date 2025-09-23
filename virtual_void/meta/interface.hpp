#pragma once

#include <utility>

#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/index_for_archetype.hpp>

namespace virtual_void::meta {

class interface {
  // archetype index -> index for v_table in i_tables
  index_for_archetype index_;

 public:
  interface() = delete;
  interface(interface const&) = delete;
  template <typename T>
  constexpr interface(std::in_place_type_t<T>) {}

};

}  // namespace virtual_void::meta