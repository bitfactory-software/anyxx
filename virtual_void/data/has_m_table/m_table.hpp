#pragma once

#include <typeindex>
#include <vector>

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data::has_m_table {

class m_table_t {
 public:
  constexpr m_table_t(const std::type_info& type_info)
      : type_info_(type_info) {}
  constexpr const std::type_info& type() const { return type_info_; }
  using m_table_target_t = void (*)();
  constexpr void set_method(int method_index, m_table_target_t target) {
    ensure_size(method_index);
    table_[method_index] = target;
  }
  template <typename TRAGET>
  constexpr void set_method(int method_index, TRAGET target) {
    auto m_table_target = reinterpret_cast<m_table_target_t>(target);
    set_method(method_index, m_table_target);
  }
  constexpr void clear() { table_.clear(); }
  constexpr auto at(int method_index, auto default_target) const {
    if (table_.size() <= method_index)
      return reinterpret_cast<m_table_target_t>(default_target);
    auto target = table_[method_index];
    if (!target) return reinterpret_cast<m_table_target_t>(default_target);
    return target;
  }
  constexpr m_table_target_t find(int method_index) const {
    if (table_.size() > method_index) return table_[method_index];
    return {};
  }

 private:
  const std::type_info& type_info_;
  std::vector<m_table_target_t> table_;
  constexpr void ensure_size(std::size_t method_index) {
    if (table_.size() <= method_index)
      table_.insert(table_.end(), 1 + method_index - table_.size(), {});
  }
};

template <typename CLASS>
constexpr m_table_t* m_table_of() {
  static m_table_t m_table_{typeid(CLASS)};
  return &m_table_;
}

}  // namespace virtual_void::data::has_m_table