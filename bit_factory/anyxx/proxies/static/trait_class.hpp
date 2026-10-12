#pragma once

#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>

namespace anyxx {

/// Proxy to capture the type to enable static
/// dispatch. Has no data member, so it doesn't capture any value and has no
/// size
///
/// \tparam Type The captured type
template <typename Type>
struct trait_class {
  using value_t = Type;
};

template <typename T>
struct is_type_class_impl : std::false_type {};
template <typename T>
struct is_type_class_impl<trait_class<T>> : std::true_type {};
template <typename T>
inline constexpr bool is_type_class =
    is_proxy<T> && is_type_class_impl<T>::value;

template <typename Type>
struct proxy_trait<trait_class<Type>> : basic_proxy_trait<trait_class<Type>> {
  using void_t = const_void;
  using static_dispatch_t = Type;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_object = false;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = true;
  };
  static constexpr bool is_owner = false;
  static constexpr bool allow_any_default_constructibile = true;

  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {}

  static auto get_proxy_ptr_in([[maybe_unused]] auto& val,
                               [[maybe_unused]] auto* v_table) {
    return nullptr;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  static auto construct_in_place(auto) {}
  template <typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {}
  template <typename Vx>
  static auto erase([[maybe_unused]] Vx&& v) {
    return trait_class<Vx>{};
  }
};

}  // namespace anyxx