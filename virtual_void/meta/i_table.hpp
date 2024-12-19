#pragma once

#include <virtual_void/data/has_i_table/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/table.hpp>

namespace virtual_void::meta {

class index_for_archetype {
  table<int, -1> table_;

 public:
  constexpr void register_archetype(int archetype_index,
                                    int index_for_v_table_in_i_table) {
    table_.register_target(archetype_index, index_for_v_table_in_i_table);
  }
  constexpr int at(int archetype_index) const {
    return table_.at(archetype_index);
  }
};

// archetype index -> index for v_table in i_tablestemplate <typename V_TABLE>
template <typename V_TABLE>
index_for_archetype& index_for_v_table_in_i_table();

template <typename V_TABLE>
index_for_archetype&
index_for_v_table_in_i_table_implementation() {
  static index_for_archetype table;
  return table;
}

class i_table {
 public:
  using v_table_ptr = interface::base_v_table*;

 private:
  table<v_table_ptr, nullptr> table_;
  const std::type_info& type_info_;
  int archetype_index_ = -1;
  using copy_construct_t = auto(const_void) -> data::has_i_table::unique;
  copy_construct_t* copy_construct_;

 public:
  template <typename CLASS>
  constexpr i_table(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()),
        archetype_index_(archetype_of<CLASS>().get_archetype_index()),
        copy_construct_(+[](const_void from) {
          return erased<data::has_i_table::unique>(
              *static_cast<CLASS const*>(from));
        }) {}
  constexpr const std::type_info& type() const { return type_info_; }
  constexpr int get_archetype_index() const { return archetype_index_; }
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
int get_i_table_idx_for(i_table const& i_table) {
  auto archetype_index = i_table.get_archetype_index();
  auto i_table_idx =
      index_for_v_table_in_i_table<V_TABLE>().at(archetype_index);
  return i_table_idx;
}

template <typename CLASS, typename V_TABLE>
struct is_a {
  constexpr is_a() {
    auto& i_table = get_i_table_of<CLASS>();
    auto i_table_idx = get_i_table_idx_for<V_TABLE>(i_table);
    if (i_table_idx < 0)
      index_for_v_table_in_i_table<V_TABLE>().register_archetype(
          i_table.get_archetype_index(), i_table_idx = i_table.size());
    using uneraser = static_cast_uneraser<CLASS>;
    auto v_table_ptr = V_TABLE::template imlpementation<uneraser>();
    i_table.register_interface(i_table_idx, v_table_ptr);
    auto size = get_i_table_of<CLASS>().size();
  }
};

}  // namespace virtual_void::meta