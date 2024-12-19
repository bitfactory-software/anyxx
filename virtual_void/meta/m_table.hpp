#pragma once

#include <typeindex>
#include <vector>
#include <virtual_void/meta/table.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

class m_table_t {
 public:
  using m_table_target_t = void (*)();

 private:
  table<m_table_target_t, nullptr> table_;
  const std::type_info& type_info_;

 public:
  constexpr m_table_t(const std::type_info& type_info)
      : type_info_(type_info) {}
  constexpr const std::type_info& type() const { return type_info_; }
  constexpr void set_method(int method_index, m_table_target_t target) {
    table_.register_target(method_index, target);
  }
  template <typename TRAGET>
  constexpr void set_method(int method_index, TRAGET target) {
    auto m_table_target = reinterpret_cast<m_table_target_t>(target);
    set_method(method_index, m_table_target);
  }
  constexpr void clear() { table_.clear(); }
  constexpr auto at(int method_index, auto default_target) const {
    if (auto target = find(method_index)) return target;
    return reinterpret_cast<m_table_target_t>(default_target);
  }
  constexpr m_table_target_t find(int method_index) const {
    return table_.at(method_index);
  }
};

template <typename CLASS>
constexpr m_table_t* m_table_of() {
  static m_table_t m_table_{typeid_of<CLASS>()};
  return &m_table_;
}

}  // namespace virtual_void::meta