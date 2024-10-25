#pragma once

#include <typeindex>
#include <vector>

#include "../../../error.h"
#include "../../../forward.h"

namespace virtual_void::erased::data::has_m_table {

class m_table_t {
 public:
  constexpr m_table_t(const std::type_info& type_info)
      : type_info_(type_info) {}
  constexpr const std::type_info& type() const { return type_info_; }
  using m_table_target_t = void (*)();
  constexpr void set_method(int method_index, m_table_target_t target) {
    ensure_size(method_index + 1);
    table_[method_index] = target;
  }
  template <typename TRAGET>
  constexpr void set_method(int method_index, TRAGET target) {
    auto m_table_target = reinterpret_cast<m_table_target_t>(target);
    set_method(method_index, m_table_target);
  }
  constexpr void clear() { table_.clear(); }
  constexpr auto at(int method_index) const { return table_.at(method_index); }
  constexpr auto operator[](int method_index) const {
    return table_[method_index];
  }

 private:
  const std::type_info& type_info_;
  std::vector<m_table_target_t> table_;
  constexpr void ensure_size(std::size_t s) {
    if (table_.size() >= s) return;
    table_.insert(table_.end(), s - table_.size(), nullptr);
  }
};
template <typename CLASS>
constexpr m_table_t* m_table_of() {
  static m_table_t m_table_{typeid(CLASS)};
  return &m_table_;
}

}  // namespace virtual_void