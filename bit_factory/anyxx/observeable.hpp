#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/small_object_size.hpp>
#include <bit_factory/anyxx/voidness.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {


/// No lifetime functionality
/// Base of v-tables for traits without lifetime requirements
struct observeable_v_table {
  using v_table_t = observeable_v_table;

  struct deduced_type {};

  using val_is_nullable = std::false_type;
  static constexpr std::size_t val_proxy_size = small_object_size;

  /// Type-erasing constructor
  template <typename Concrete, typename Parent = observeable_v_table>
  explicit observeable_v_table(
      [[maybe_unused]] std::in_place_type_t<Concrete> concrete,
      [[maybe_unused]] std::in_place_type_t<Parent> parent =
          std::in_place_type<Parent>) {}

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(observeable_v_table) == from;
  }
};

template <typename T = std::nullptr_t>
struct observeable_default_trait_map {
  using rep_type = std::nullptr_t;
  struct deduced_type {};
};

template <typename T = std::nullptr_t>
struct observeable_map : observeable_default_trait_map<T> {};

template <typename = std::nullptr_t>
struct observeable {
  template <typename>
  static constexpr bool modeled_by() {
    return true;
  }

  using default_proxy_t = cref;

  template <typename StaticDispatchType>
  using static_dispatch_map_t = observeable_map<StaticDispatchType>;

  template <typename T>
  using trait_map = observeable_map<T>;

  using v_table_t = observeable_v_table;
  using val_is_nullable = typename v_table_t::val_is_nullable;
  static constexpr std::size_t val_proxy_size = v_table_t::val_proxy_size;
};


}  // namespace anyxx