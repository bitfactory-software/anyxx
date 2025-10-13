#pragma once

#include <expected>
#include <map>
#include <optional>
#include <ranges>
#include <type_traits>
#include <typeindex>
#include <variant>
#include <vector>
#include <anypp/observer.hpp>
#include <anypp/unique.hpp>
#include <anypp/virtual_void.hpp>

namespace anypp {

class meta_data;
struct any_base_v_table;

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data{std::in_place_type<TYPE>};
  return meta_data;
}

#ifdef VV_DLL_MODE
template <typename T>
const std::type_info& typeid_of();
template <typename T>
meta_data& get_meta_data();
#else
template <typename T>
const std::type_info& typeid_of() {
  return typeid(T);
}
template <typename T>
meta_data& get_meta_data() {
  return runtime_implementation<std::decay_t<T>>();
}
#endif

struct any_base_v_table {
  template <typename CONCRETE>
  any_base_v_table(std::in_place_type_t<CONCRETE> concrete);

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(any_base_v_table) == from;
  }

  meta_data* meta_data = nullptr;

  bool (*_is_derived_from)(const std::type_info&);
};

inline bool is_derived_from(const std::type_info& from,
                            any_base_v_table const* any_base_v_table) {
  return any_base_v_table->_is_derived_from(from);
}

using extension_method_table_function_t = void (*)();
using extension_method_table_dispatch_index_t = std::size_t;
using extension_method_table_entry_t =
    std::variant<std::monostate, extension_method_table_dispatch_index_t,
                 extension_method_table_function_t>;
using extension_method_table_t = std::vector<extension_method_table_entry_t>;

void insert_function(extension_method_table_t* v_table, std::size_t index,
                     auto fp) {
  if (v_table->size() <= index) v_table->resize(index + 1);
  v_table->at(index) =
      reinterpret_cast<extension_method_table_function_t>(fp);
}
inline extension_method_table_function_t get_function(
    extension_method_table_t* v_table, std::size_t index) {
  if (v_table->size() <= index) return {};
  if (auto f =
          std::get_if<extension_method_table_function_t>(&v_table->at(index)))
    return *f;
  else
    return {};
}

inline std::optional<std::size_t> get_multi_method_index_at(
    extension_method_table_t* v_table, std::size_t index) {
  if (v_table->size() <= index) return {};
  if (auto extension_method_table_dispatch_index =
          std::get_if<extension_method_table_dispatch_index_t>(
              &v_table->at(index)))
    return *extension_method_table_dispatch_index;
  return {};
}
inline void set_multi_method_index_at(
    extension_method_table_t* v_table, std::size_t index_multi_method,
    extension_method_table_dispatch_index_t
        dispatch_index_of_class_in_dispatch_matrix) {
  if (v_table->size() <= index_multi_method)
    v_table->resize(index_multi_method + 1);
  v_table->at(index_multi_method) = dispatch_index_of_class_in_dispatch_matrix;
}

struct cast_error {
  std::type_info const &to, &from;
};

class meta_data {
  const std::type_info& type_info_;
  using copy_construct_t = auto(const_void) -> unique;
  copy_construct_t* copy_construct_;

  std::vector<any_base_v_table*> i_table_;

 public:
  template <typename CLASS>
  constexpr meta_data(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()), copy_construct_(+[](const_void from) {
          return erased<unique>(
              std::make_unique<CLASS>(*static_cast<CLASS const*>(from)));
        }) {}

  constexpr const std::type_info& get_type_info() const { return type_info_; }
  auto copy_construct(const_void from) const { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  std::expected<any_base_v_table*, cast_error> get_v_table(
      std::type_info const& typeid_) const {
    auto& i_table = get_i_table();
    for (auto v_table : i_table)
      if (is_derived_from(typeid_, v_table)) return v_table;
    return std::unexpected(cast_error{.to = typeid_, .from = get_type_info()});
  }
  auto register_v_table(any_base_v_table* v_table) {
    v_table->meta_data = this;
    if (std::ranges::find(get_i_table(), v_table) == get_i_table().end())
      i_table_.push_back(v_table);
    return v_table;
  }
};

template <typename CONCRETE>
any_base_v_table::any_base_v_table(std::in_place_type_t<CONCRETE> concrete)
    : _is_derived_from([](const std::type_info& from) {
        return static_is_derived_from(from);
      }) {}

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance_implementation() {
  static extension_method_table_t extension_method_table;
  return &extension_method_table;
}

#ifdef VV_DLL_MODE

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance();

#else

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance() {
  return extension_method_table_instance_implementation<EXTENDED_V_TABLE,
                                                        CLASS_NAME>();
}

#endif  //

template <typename V_TABLE, typename CONCRETE>
auto bind_v_table_to_meta_data() {
  auto v_table = V_TABLE::template imlpementation<CONCRETE>();
  get_meta_data<CONCRETE>().register_v_table(v_table);
  return v_table;
}

}  // namespace anypp

namespace anypp {
template <typename U>
bool type_match(meta_data const& meta_data) {
  return &meta_data.get_type_info() == &typeid_of<std::decay_t<U>>();
}
}  // namespace anypp

#ifdef VV_DLL_MODE

#define VV_RUNTIME_FWD(export_, ...)               \
  template <>                                      \
  export_ const std::type_info&                    \
  anypp::typeid_of<__VA_ARGS__>(); \
  template <>                                      \
  export_ anypp::meta_data&        \
  anypp::get_meta_data<__VA_ARGS__>();

#define VV_RUNTIME_INSTANCE(...)                                          \
  template <>                                                             \
  const std::type_info& anypp::typeid_of<__VA_ARGS__>() { \
    return typeid(__VA_ARGS__);                                           \
  }                                                                       \
  template <>                                                             \
  anypp::meta_data&                                       \
  anypp::get_meta_data<std::decay_t<__VA_ARGS__>>() {     \
    return runtime_implementation<__VA_ARGS__>();                         \
  }

#else

#define VV_RUNTIME_FWD(...)
#define VV_RUNTIME_INSTANCE(...)

#endif

#define VV_RUNTIME_STATIC(...)  \
  VV_RUNTIME_FWD(, __VA_ARGS__) \
  VV_RUNTIME_INSTANCE(__VA_ARGS__)\
