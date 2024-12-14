#pragma once

#include <typeindex>
#include <vector>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::interface {

template <is_constness CONSTNESS>
class i_table_variant {
 public:
  using i_table_target_t = v_table_base<void_t<CONSTNESS>>*;

 private:
  std::vector<i_table_target_t> table_;

 public:
  constexpr void register_interface(int index, i_table_target_t target) {
    ensure_size(index);
    table_[index] = target;
  }
  constexpr void clear() { table_.clear(); }
  constexpr i_table_target_t at(int v_table_index) const {
    if (table_.size() <= v_table_index) return {};
    auto target = table_[v_table_index];
    return target;
  }
  constexpr i_table_target_t find(int v_table_index) const {
    if (table_.size() > v_table_index) return table_[v_table_index];
    return {};
  }
  constexpr void ensure_size(std::size_t v_table_index) {
    if (table_.size() <= v_table_index)
      table_.insert(table_.end(), 1 + v_table_index - table_.size(), {});
  }
};

class i_table {
 public:
 private:
  const std::type_info& type_info_;
  i_table_variant<const_> const_table_;
  i_table_variant<mutable_> mutable_table_;

 public:
  constexpr i_table(const std::type_info& type_info) : type_info_(type_info) {}
  constexpr const std::type_info& type() const { return type_info_; }
  constexpr i_table_variant<const_>* get_const() {
    return &const_table_;
  }
  constexpr i_table_variant<const_> const* get_const() const {
    return &const_table_;
  }
  constexpr i_table_variant<mutable_>* get_mutable() {
    return &mutable_table_;
  }
  constexpr i_table_variant<mutable_> const* get_mutable() const {
    return &mutable_table_;
  }

  template <is_constness CONSTNESS>
  interface::i_table_variant<CONSTNESS> const* get() const {
    if constexpr (std::same_as<CONSTNESS, const_>) {
      return get_const();
    } else {
      get_mutable();
    }
  }

  template <is_constness CONSTNESS>
  interface::i_table_variant<CONSTNESS>* get() {
    if constexpr (std::same_as<CONSTNESS, const_>) {
      return get_const();
    } else {
      get_mutable();
    }
  }
};

template <typename CLASS>
constexpr i_table* i_table_of() {
  static i_table table{typeid_of<CLASS>()};
  return &table;
}

}  // namespace virtual_void::interface