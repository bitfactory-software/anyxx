#pragma once

#include <virtual_void/meta/table.hpp>

namespace virtual_void::meta {

class index_for_archetype : table<int, -1> {
 public:
  int operator()(archetype_t& archetype, int archetype_t::*registered) {
    auto arechetype_idx = archetype.get_archetype_index();
    if (auto idx = at(arechetype_idx); idx >= 0) return idx;
    return register_target(arechetype_idx, (archetype.*registered)++);
  }
  int operator()(archetype_t& archetype) const {
    return at(archetype.get_archetype_index());
  }
};
}  // namespace virtual_void::meta