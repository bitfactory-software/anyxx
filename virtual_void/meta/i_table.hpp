#pragma once

#include <virtual_void/meta/forward.hpp>
#include <virtual_void/data/has_i_table/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

class i_table {
 public:
  using i_table_target_t = interface::base_v_table*;

private:
  const std::type_info& type_info_;
  std::vector<i_table_target_t> table_;
  using copy_construct_t = auto(const_void) -> data::has_i_table::unique;
  copy_construct_t* copy_construct_;

 public:
  template <typename CLASS>
  constexpr i_table(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()), copy_construct_(+[](const_void from) {
          return erased<data::has_i_table::unique>(
              *static_cast<CLASS const*>(from));
        }) {}
  constexpr const std::type_info& type() const { return type_info_; }
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

  auto copy_construct(const_void from) { return copy_construct_(from); }
};

constexpr const std::type_info& get_type_info(i_table const* t) {
  return t->type();
}

template <typename CLASS>
struct i_table_implementation_of {
  constexpr i_table* operator()() const {
    static i_table table{std::in_place_type<CLASS>};
    return &table;
  }
};

template <typename CLASS>
struct i_table_of;

template <typename CLASS>
constexpr i_table* get_i_table_of() {
  return i_table_of<CLASS>{}();
}

template <typename CLASS, typename V_TABLE>
struct is_a {
  constexpr is_a() {
    auto i_table_index_ = i_table_index<V_TABLE>();
    auto i_table_ = get_i_table_of<CLASS>();
    using uneraser = static_cast_uneraser<CLASS>;
    auto v_table_ = V_TABLE::template imlpementation<uneraser>();
    i_table_->register_interface(i_table_index_, v_table_);
  }
};

}  // namespace virtual_void::meta