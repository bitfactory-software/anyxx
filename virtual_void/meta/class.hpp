#pragma once

#include <map>
#include <ranges>
#include <typeindex>
#include <vector>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/utillities/overload.hpp>
#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

class runtime_t;
struct base_v_table;

template <typename TYPE>
runtime_t& runtime();

template <typename TYPE>
auto& runtime_implementation() {
  static runtime_t runtime{std::in_place_type<TYPE>};
  return runtime;
}


struct base_v_table {
  template <typename CONCRETE>
  base_v_table(std::in_place_type_t<CONCRETE> concrete);

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(base_v_table) == from;
  }

  runtime_t* type_info = nullptr;

  bool (*_is_derived_from)(const std::type_info&);

  base_v_table(auto unused)
      : _is_derived_from([](const std::type_info& from) {
          return static_is_derived_from(from);
        }) {};
};

inline bool is_derived_from(const std::type_info& from,
                            meta::base_v_table const* base_v_table) {
  return base_v_table->_is_derived_from(from);
}

template <typename CONCRETE>
base_v_table* base_v_table_imlpementation() {
  static base_v_table v_table{std::in_place_type<CONCRETE>};
  return &v_table;
}

class runtime_t {
  const std::type_info& type_info_;
  using copy_construct_t = auto(const_void) -> data::unique;
  copy_construct_t* copy_construct_;

  std::vector<base_v_table*> i_table_;

 public:
  template <typename CLASS>
  constexpr runtime_t(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()), copy_construct_(+[](const_void from) {
          return erased<data::unique>(*static_cast<CLASS const*>(from));
        }) {}

  constexpr operator const std::type_info&() const { return get_type_info(); }
  constexpr const std::type_info& get_type_info() const { return type_info_; }
  auto copy_construct(const_void from) const { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  base_v_table* get_v_table(std::type_info const& typeid_) const {
    auto& i_table = get_i_table();
    for (auto v_table : i_table)
      if (is_derived_from(typeid_, v_table)) return v_table;
    return nullptr;
  }
  void register_v_table(base_v_table* v_table) {
    v_table->type_info = this;
    if (std::ranges::find(get_i_table(), v_table) == get_i_table().end())
      i_table_.push_back(v_table);
  }
};

template <typename CONCRETE>
base_v_table::base_v_table(std::in_place_type_t<CONCRETE> concrete) {
  meta::runtime<CONCRETE>().register_v_table(this);
}

template <typename CLASS, typename V_TABLE>
struct is_a {
  constexpr is_a() {
    auto& type_info = runtime<CLASS>();
    auto v_table_ptr = V_TABLE::template imlpementation<CLASS>();
  };
};
}  // namespace virtual_void::meta

namespace virtual_void {
template <typename U>
bool type_match(meta::runtime_t const& meta) {
  return &meta.get_type_info() == &typeid_of<U>();
}
}  // namespace virtual_void

#define VV_RUNTIME(export_, ...)         \
  template <>                            \
  export_ virtual_void::meta::runtime_t& \
  virtual_void::meta::runtime<__VA_ARGS__>();

#define VV_RUNTIME_IMPEMENTATION(...)                                         \
  template <>                                                                 \
  virtual_void::meta::runtime_t& virtual_void::meta::runtime<__VA_ARGS__>() { \
    return runtime_implementation<__VA_ARGS__>();                             \
  }

#define VV_RUNTIME_STATIC(...) \
  VV_RUNTIME(, __VA_ARGS__)    \
  VV_RUNTIME_IMPEMENTATION(__VA_ARGS__)\
