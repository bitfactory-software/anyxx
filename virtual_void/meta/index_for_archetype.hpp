#pragma once

#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/table.hpp>

namespace virtual_void::meta {

class index_for_archetype : table<int, -1> {
 public:
  int operator()(archetype& archetype, int archetype::*registered) {
    if (auto idx = (*this)(archetype); idx >= 0) return idx;
    return register_at(archetype, (archetype.*registered)++);
  }
  int register_at(archetype& archetype, int index) {
    return register_target(archetype.get_archetype_index(), index);
  }
  int operator()(archetype& archetype) const {
    return at(archetype.get_archetype_index());
  }
};
}  // namespace virtual_void::meta