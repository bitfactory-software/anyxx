#pragma once

#include <typeindex>
#include <vector>

#include <virtual_void/virtual_void.hpp>
#include <virtual_void/interface/base.hpp>

namespace virtual_void::interface {

template<is_constness CONSTNESS> 
class i_table {
 public:
  using i_table_target_t = v_table_base<void_t<CONSTNESS>>*;
 private:
  const std::type_info& type_info_;
  std::vector<i_table_target_t> table_;
 public:
  constexpr i_table(const std::type_info& type_info)
      : type_info_(type_info) {}
  constexpr const std::type_info& type() const { return type_info_; }
  constexpr void register_interface(int index, i_table_target_t target) {
    ensure_size(index);
    table_[index] = target;
  }
  constexpr void clear() { table_.clear(); }
  constexpr i_table_target_t at(int method_index) const {
    if (table_.size() <= method_index)
      return {};
    auto target = table_[method_index];
    return target;
  }
  constexpr i_table_target_t find(int method_index) const {
    if (table_.size() > method_index) return table_[method_index];
    return {};
  }
  constexpr void ensure_size(std::size_t method_index) {
    if (table_.size() <= method_index)
      table_.insert(table_.end(), 1 + method_index - table_.size(), {});
  }
};

template <typename CLASS, is_constness CONSTNESS>
constexpr i_table<CONSTNESS>* i_table_of() {
  static i_table<CONSTNESS> table{typeid_of<CLASS>()};
  return &table;
}

}  // namespace virtual_void::data::has_i_table