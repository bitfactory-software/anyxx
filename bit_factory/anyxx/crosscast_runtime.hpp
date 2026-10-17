#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/errors/cast_error.hpp>
#include <bit_factory/anyxx/v_table_instance.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>
#include <bit_factory/anyxx/ANY_UNPAREN.hpp>

#include <expected>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <ranges>

namespace anyxx {

class meta_data;

template <typename TYPE>
auto& runtime_implementation();

#ifdef ANY_DLL_MODE
template <typename T>
meta_data& get_meta_data();
#else
template <typename T>
meta_data& get_meta_data() {
  return runtime_implementation<std::decay_t<T>>();
}
#endif

template <is_any ToAny, is_dynamic_castable_v_table FromVTable>
  requires is_dynamic_castable_v_table<typename ToAny::v_table_t>
auto query_v_table(FromVTable* from)
    -> std::expected<typename ToAny::v_table_t*, anyxx::cast_error> {
  using v_table_t = typename ToAny::v_table_t;
  if (from->is_derived_from_(typeid(v_table_t)))
    return reinterpret_cast<v_table_t*>(from);
  if constexpr (is_meta_data_v_table<FromVTable>) {
    if (auto meta_data = from->meta_data_; meta_data) {
      return meta_data->template get_v_table<v_table_t>();
    } else {
      return std::unexpected(
          anyxx::cast_error{typeid(v_table_t), *from->type_info_});
    }
  } else {
    return std::unexpected(
        anyxx::cast_error{typeid(v_table_t), *from->type_info_});
  }
}

class meta_data {
  const std::type_info& type_info_;

  struct i_table_entry {
    void* v_table_ = nullptr;
    std::type_index type_index_;

    template <typename VTable>
    i_table_entry(VTable* v_table)
        : v_table_(v_table), type_index_(typeid(*v_table)) {}
  };

  std::vector<i_table_entry> i_table_;

 public:
  template <typename CLASS>
  explicit constexpr meta_data(std::in_place_type_t<CLASS>)
      : type_info_(typeid(CLASS)) {}

  constexpr const std::type_info& type() const { return type_info_; }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  template <typename VTable>
  VTable* find_v_table() const {
    if (auto found =
            std::ranges::find(get_i_table(), std::type_index(typeid(VTable)),
                              &i_table_entry::type_index_);
        found != get_i_table().end())
      return static_cast<VTable*>(found->v_table_);
    return nullptr;
  }

  template <typename VTable>
  std::expected<VTable*, cast_error> get_v_table() const {
    if (auto v_table = find_v_table<VTable>(); v_table) return v_table;
    return std::unexpected(
        cast_error{.to = typeid(VTable), .from = type()});
  }
  template <typename VTable>
  auto register_v_table(VTable* v_table) {
    v_table->meta_data_ = this;
    if (!find_v_table<VTable>()) i_table_.push_back({v_table});
    return v_table;
  }
};

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data_{std::in_place_type<TYPE>};
  return meta_data_;
}

template <typename VTable, typename Concrete>
auto bind_v_table_to_meta_data() {
    auto v_table = v_table_instance<VTable, Concrete>();
    get_meta_data<Concrete>().register_v_table(v_table);
    return v_table;
}

}  // namespace anyxx

#ifdef ANY_DLL_MODE

#define ANY_META_CLASS_FWD(export_, ...) \
  template <>                            \
  export_ anyxx::meta_data& anyxx::get_meta_data<__VA_ARGS__>();

#define ANY_META_CLASS(...)                                             \
  template <>                                                           \
  anyxx::meta_data& anyxx::get_meta_data<std::decay_t<__VA_ARGS__>>() { \
    return runtime_implementation<__VA_ARGS__>();                       \
  }

#else

/// \addtogroup meta_class_macros ANY_META_CLASS macros for crosscast
/// \brief Macros to define static runtime type data for a \ref
/// model. Only neccessary for DLL \b and crosscast scenarios.
/// @{

/// \def ANY_META_CLASS_FWD
/// \brief Declare access to the meta data for a specific model any_.
/// Must be in global namespace.
/// \param export_ To supply an export macro in a DLL scenario
/// \param ... Type of the model
#define ANY_META_CLASS_FWD(...)
/// \def ANY_META_CLASS
/// \brief Define the meta data for a specific model any_. Must be in
/// global namespace.
/// \param ... Type of the model
#define ANY_META_CLASS(...)

#endif

#define ANY_META_CLASS_STATIC(...)  \
  ANY_META_CLASS_FWD(, __VA_ARGS__) \
  ANY_META_CLASS(__VA_ARGS__)

/// \def ANY_REGISTER_MODEL
/// \brief Register a model class for a specific any_ interface. Must
/// be in global namespace.
/// \param class_ The model class with fully qualified name. Must be
/// parenthesized
/// \param interface_ Name of the \ref any_ (without any prefix).
/// \param ... Optional template parameters for the model class.
///
/// See also \ref casts.
#define ANY_REGISTER_MODEL(class_, interface_, ...)                           \
  namespace {                                                                 \
  static auto __ = anyxx::bind_v_table_to_meta_data<                          \
      interface_##_v_table _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(__VA_ARGS__), \
      ANY_UNPAREN(class_)>();                                               \
  }
/// @}
