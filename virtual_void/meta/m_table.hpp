#pragma once

#include <typeindex>
#include <vector>
#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/index_for_archetype.hpp>
#include <virtual_void/meta/table.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

// archetype index -> index for method in m_tables
template <typename METHOD>
index_for_archetype& index_for_method_in_m_table();

template <typename METHOD>
index_for_archetype& index_for_method_in_m_table_implementation() {
  static index_for_archetype table;
  return table;
}

class m_table_t {
 public:
  using m_table_target_t = void (*)();

 private:
  table<m_table_target_t, nullptr> table_;
  const std::type_info& type_info_;
  archetype_t& archetype_;

 public:
  m_table_t() = delete;
  m_table_t(const m_table_t&) = delete;
  template <typename CLASS>
  constexpr m_table_t(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()), archetype_(archetype_of<CLASS>()) {}
  constexpr const std::type_info& type() const { return type_info_; }
  constexpr int get_archetype_index() const {
    return archetype_.get_archetype_index();
  }
  constexpr archetype_t& get_archetype() const { return archetype_; }
  constexpr void set_method(int method_index, m_table_target_t target) {
    table_.register_target(method_index, target);
  }
  template <typename TRAGET>
  constexpr void set_method(int method_index, TRAGET target) {
    auto m_table_target = reinterpret_cast<m_table_target_t>(target);
    set_method(method_index, m_table_target);
  }
  constexpr auto at(int method_index, auto default_target) const {
    if (auto target = table_.at(method_index)) return target;
    return reinterpret_cast<m_table_target_t>(default_target);
  }
  constexpr auto is_defined(int method_index) const {
    return table_.is_defined(method_index);
  }
  constexpr auto size() const { return table_.size(); }
};

template <typename CLASS>
constexpr m_table_t* m_table_of() {
  static m_table_t m_table_{std::in_place_type<CLASS>};
  return &m_table_;
}

}  // namespace virtual_void::meta
