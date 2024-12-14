#pragma once

#include <typeindex>
#include <vector>

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data::has_i_table {

class i_table_t {
 public:
  constexpr i_table_t()
      : type_info_(type_info) {}
  constexpr const std::type_info& type() const { return type_info_; }
  using i_table_target_t = void (*)();
  constexpr void set_method(int method_index, i_table_target_t target) {
    ensure_size(method_index);
    table_[method_index] = target;
  }
  template <typename TRAGET>
  constexpr void set_method(int method_index, TRAGET target) {
    auto i_table_target = reinterpret_cast<i_table_target_t>(target);
    set_method(method_index, i_table_target);
  }
  constexpr void clear() { table_.clear(); }
  constexpr auto at(int method_index, auto default_target) const {
    if (table_.size() <= method_index)
      return reinterpret_cast<i_table_target_t>(default_target);
    auto target = table_[method_index];
    if (!target) return reinterpret_cast<i_table_target_t>(default_target);
    return target;
  }
  constexpr i_table_target_t find(int method_index) const {
    if (table_.size() > method_index) return table_[method_index];
    return {};
  }

 private:
  const std::type_info& type_info_;
  std::vector<i_table_target_t> table_;
  constexpr void ensure_size(std::size_t method_index) {
    if (table_.size() <= method_index)
      table_.insert(table_.end(), 1 + method_index - table_.size(), {});
  }
};

template <typename CLASS>
constexpr i_table_t* i_table_of() {
  static i_table_t i_table_{typeid_of<CLASS>()};
  return &i_table_;
}

}  // namespace virtual_void::data::has_i_table