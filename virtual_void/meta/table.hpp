#pragma once

#include <virtual_void/meta/forward.hpp>

namespace virtual_void::meta {

template <typename TRAGET, auto DEFAULT>
class table {
  std::vector<TRAGET> table_;
  constexpr void ensure_size(std::size_t index) {
    if (table_.size() <= index)
      table_.insert(table_.end(), 1 + index - table_.size(), DEFAULT);
  }

 public:
  constexpr table() = default;
  constexpr table(table const&) = delete;
  constexpr void register_target(int index, TRAGET target) {
    ensure_size(index);
    table_[index] = target;
  }
  constexpr void clear() { table_.clear(); }

  constexpr TRAGET at(int index) const {
    if (table_.size() <= index) return DEFAULT;
    auto target = table_[index];
    return target;
  }
  constexpr TRAGET find(int index) const {
    if (table_.size() > index) return table_[index];
    return DEFAULT;
  }
  constexpr auto size() const { return table_.size(); }
};

}  // namespace virtual_void::meta