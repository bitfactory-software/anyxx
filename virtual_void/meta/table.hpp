#pragma once

#include <optional>
#include <vector>
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
  constexpr auto register_target(int index, TRAGET target) {
    ensure_size(index);
    table_[index] = target;
    return target;
  }
  constexpr void clear() { table_.clear(); }

  constexpr TRAGET at(int index) const { return at(index, DEFAULT); }
  constexpr TRAGET at(int index, auto default_target) const {
    if (0 <= index && index < table_.size())
      if (auto found = table_[index]; found != DEFAULT) return found;
    return default_target;
  }

  constexpr std::optional<TRAGET> is_defined(int index) const {
    if (auto found = at(index); found != DEFAULT) return found;
    return {};
  };

  constexpr auto size() const { return table_.size(); }
};

}  // namespace virtual_void::meta