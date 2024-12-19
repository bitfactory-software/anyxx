#pragma once

#include <virtual_void/meta/table.hpp>

namespace virtual_void::meta {

class index_for_archetype : table<int, -1> {

 public:
  constexpr void register_archetype(int archetype_index, int target_index) {
    register_target(archetype_index, target_index);
  }
  using table::at;
};
}  // namespace virtual_void::meta