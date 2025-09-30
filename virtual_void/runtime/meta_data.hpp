#pragma once

#include <expected>
#include <map>
#include <ranges>
#include <type_traits>
#include <typeindex>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::runtime {

template <typename T>
const std::type_info& typeid_of();

class meta_data;
struct base_v_table;

template <typename TYPE>
meta_data& get_meta_data();

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data{std::in_place_type<TYPE>};
  return meta_data;
}

struct base_v_table {
  template <typename CONCRETE>
  base_v_table(std::in_place_type_t<CONCRETE> concrete);

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(base_v_table) == from;
  }

  meta_data* meta_data = nullptr;

  bool (*_is_derived_from)(const std::type_info&);
};

inline bool is_derived_from(const std::type_info& from,
                            runtime::base_v_table const* base_v_table) {
  return base_v_table->_is_derived_from(from);
}

using extension_method_table_function_t = void (*)();
using extension_method_table_t = std::vector<extension_method_table_function_t>;

void insert_function(extension_method_table_t* v_table, std::size_t index,
                     auto fp) {
  if (v_table->size() <= index) v_table->resize(index + 1);
  v_table->at(index) =
      reinterpret_cast<runtime::extension_method_table_function_t>(fp);
}

template <typename CONCRETE>
base_v_table* base_v_table_imlpementation() {
  static base_v_table v_table{std::in_place_type<CONCRETE>};
  return &v_table;
}

struct cast_error {
  std::type_info const &to, &from;
};

class meta_data {
  const std::type_info& type_info_;
  using copy_construct_t = auto(const_void) -> data::unique;
  copy_construct_t* copy_construct_;

  std::vector<base_v_table*> i_table_;

 public:
  template <typename CLASS>
  constexpr meta_data(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()), copy_construct_(+[](const_void from) {
          return data::erased<data::unique>(
              std::make_unique<CLASS>(*static_cast<CLASS const*>(from)));
        }) {}

  constexpr const std::type_info& get_type_info() const { return type_info_; }
  auto copy_construct(const_void from) const { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  std::expected<base_v_table*, cast_error> get_v_table(
      std::type_info const& typeid_) const {
    auto& i_table = get_i_table();
    for (auto v_table : i_table)
      if (is_derived_from(typeid_, v_table)) return v_table;
    return std::unexpected(cast_error{.to = typeid_, .from = get_type_info()});
  }
  void register_v_table(base_v_table* v_table) {
    v_table->meta_data = this;
    if (std::ranges::find(get_i_table(), v_table) == get_i_table().end())
      i_table_.push_back(v_table);
  }
};

template <typename CONCRETE>
base_v_table::base_v_table(std::in_place_type_t<CONCRETE> concrete)
    : _is_derived_from([](const std::type_info& from) {
        return static_is_derived_from(from);
      }) {
  runtime::get_meta_data<CONCRETE>().register_v_table(this);
}

template <typename CLASS>
struct class_ {
  template <template <data::is_erased_data> typename INTERFACE>
  struct implements {
    constexpr implements() {
      auto& type_info = get_meta_data<CLASS>();
      auto v_table_ptr = INTERFACE<
          data::const_observer>::template v_table_imlpementation<CLASS>();
    };
  };
};

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance() {
  static extension_method_table_t extension_method_table;
  return &extension_method_table;
}

}  // namespace virtual_void::runtime

namespace virtual_void {
template <typename U>
bool type_match(runtime::meta_data const& meta_data) {
  return &meta_data.get_type_info() == &runtime::typeid_of<std::decay_t<U>>();
}
}  // namespace virtual_void

#define VV_RUNTIME(export_, ...)                   \
  template <>                                      \
  export_ const std::type_info&                    \
  virtual_void::runtime::typeid_of<__VA_ARGS__>(); \
  template <>                                      \
  export_ virtual_void::runtime::meta_data&        \
  virtual_void::runtime::get_meta_data<__VA_ARGS__>();

#define VV_RUNTIME_IMPEMENTATION(...)                                     \
  template <>                                                             \
  const std::type_info& virtual_void::runtime::typeid_of<__VA_ARGS__>() { \
    return typeid(__VA_ARGS__);                                           \
  }                                                                       \
  template <>                                                             \
  virtual_void::runtime::meta_data&                                       \
  virtual_void::runtime::get_meta_data<std::decay_t<__VA_ARGS__>>() {     \
    return runtime_implementation<__VA_ARGS__>();                         \
  }

#define VV_RUNTIME_STATIC(...) \
  VV_RUNTIME(, __VA_ARGS__)    \
  VV_RUNTIME_IMPEMENTATION(__VA_ARGS__)\
