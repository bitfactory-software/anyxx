#pragma once

#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/meta/interface.hpp>
#include <virtual_void/meta/table.hpp>

namespace virtual_void::meta {

class type_info {
  const std::type_info& type_info_;
  archetype& archetype_;
  using copy_construct_t = auto(const_void) -> data::has_meta_runtime::unique;
  copy_construct_t* copy_construct_;

  table<virtual_void::interface::base_v_table*, nullptr> i_table_;
  table<void (*)(), nullptr> m_table_;

 public:
  template <typename CLASS>
  constexpr type_info(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()),
        archetype_(runtime<archetype, archetype_for_class<CLASS>>()),
        copy_construct_(+[](const_void from) {
          return erased<data::has_meta_runtime::unique>(
              *static_cast<CLASS const*>(from));
        }) {}

  constexpr operator const std::type_info&() const { return get_type_info(); }
  constexpr const std::type_info& get_type_info() const { return type_info_; }
  constexpr archetype& get_archetype() const { return archetype_; }
  constexpr int get_archetype_index() const {
    return get_archetype().get_archetype_index();
  }
  auto copy_construct(const_void from) { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_m_table() { return m_table_; }
  auto& get_i_table() const { return i_table_; }
  auto& get_m_table() const { return m_table_; }
};

}  // namespace virtual_void::meta
