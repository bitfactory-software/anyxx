#pragma once

#include <virtual_void/data/has_i_table/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/meta/interface.hpp>
#include <virtual_void/meta/table.hpp>

namespace virtual_void::meta {

class i_table {
 public:
  using v_table_ptr = virtual_void::interface::base_v_table*;

 private:
  table<v_table_ptr, nullptr> table_;
  const std::type_info& type_info_ = typeid(nullptr);
  archetype& archetype_;
  using copy_construct_t = auto(const_void) -> data::has_i_table::unique;
  copy_construct_t* copy_construct_;

 public:
  template <typename CLASS>
  constexpr i_table(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()),
        archetype_(runtime<archetype, archetype_for_class<CLASS>>()),
        copy_construct_(+[](const_void from) {
          return erased<data::has_i_table::unique>(
              *static_cast<CLASS const*>(from));
        }) {}
  constexpr const std::type_info& type() const { return type_info_; }
  constexpr int get_archetype_index() const {
    return archetype_.get_archetype_index();
  }
  constexpr archetype& get_archetype() const { return archetype_; }
  auto copy_construct(const_void from) { return copy_construct_(from); }

  constexpr void register_interface(int v_table_index, v_table_ptr v_table) {
    table_.register_target(v_table_index, v_table);
  }
  constexpr v_table_ptr at(int v_table_index) const {
    return table_.at(v_table_index);
  }
  constexpr auto size() const { return table_.size(); }
};

constexpr const std::type_info& get_type_info(i_table const& t) {
  return t.type();
}

template <typename CLASS>
struct i_table_implementation_of {
  static constexpr i_table& instance() {
    static i_table table{std::in_place_type<CLASS>};
    return table;
  }
};

template <typename CLASS>
struct i_table_of;

template <typename CLASS>
constexpr i_table& get_i_table_of() {
  return i_table_of<CLASS>::instance();
}

template <typename V_TABLE>
int archetype_index_in_v_table(archetype& archetype);
template <>
inline int archetype_index_in_v_table<base_v_table>(archetype& archetype) {
  return -1;
}
template <typename V_TABLE>
int archetype_index_in_v_table(archetype& archetype) {
  using base_t = V_TABLE::v_table_base_t;
  auto index = archetype_index_in_v_table<base_t>(archetype);
  auto& interface_meta = runtime<meta::interface, V_TABLE>();
  if (index < 0)
    return interface_meta.register_archetype(archetype);
  else
    return interface_meta.register_archetype_at(archetype, index);
}

template <typename CLASS, typename V_TABLE>
struct is_a {
  constexpr is_a() {
    auto& i_table = get_i_table_of<CLASS>();
    auto& archetype = i_table.get_archetype();
    auto i_table_idx = archetype_index_in_v_table<V_TABLE>(archetype);
    using uneraser = static_cast_uneraser<CLASS>;
    auto v_table_ptr = V_TABLE::template imlpementation<uneraser>();
    i_table.register_interface(i_table_idx, v_table_ptr);
  }
};

}  // namespace virtual_void::meta