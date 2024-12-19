#pragma once

#include <virtual_void/meta/forward.hpp>

namespace virtual_void::meta {

template <typename TRAGET, auto DEFAULT>
class table {
  std::vector<TRAGET> table_;
  constexpr void ensure_size(std::size_t v_table_index) {
    if (table_.size() <= v_table_index)
      table_.insert(table_.end(), 1 + v_table_index - table_.size(), DEFAULT);
  }

 public:
  constexpr table() = default;
  constexpr table(table const&) = delete;
  constexpr void register_target(int index, TRAGET target) {
    ensure_size(index);
    table_[index] = target;
  }
  constexpr void clear() { table_.clear(); }

  constexpr TRAGET at(int v_table_index) const {
    if (table_.size() <= v_table_index) return DEFAULT;
    auto target = table_[v_table_index];
    return target;
  }
  constexpr TRAGET find(int v_table_index) const {
    if (table_.size() > v_table_index) return table_[v_table_index];
    return DEFAULT;
  }
  constexpr auto size() const { return table_.size(); }
};

}  // namespace virtual_void::meta