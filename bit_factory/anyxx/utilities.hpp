#pragma once

#include <bit_factory/infra/config.hpp>
#include <variant>
#include <type_traits>

namespace anyxx {

template <typename, typename = void>
struct is_type_complete_impl : std::false_type {};
template <typename T>
struct is_type_complete_impl<
    T, std::enable_if_t<std::is_object<T>::value &&
                        !std::is_pointer<T>::value && (sizeof(T) > 0)>>
    : std::true_type {};
template <typename T>
constexpr static inline bool is_type_complete = is_type_complete_impl<T>::value;

template <typename T, typename Rep>
using default_rep = std::conditional_t<is_type_complete<Rep>, Rep, T>;

template <class... Ts>
struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename T>
struct is_variant_impl : std::false_type {};
template <typename... Args>
struct is_variant_impl<std::variant<Args...>> : std::true_type {};
template <typename T>
inline constexpr bool is_variant = is_variant_impl<T>::value;

inline constexpr bool ignore_mapf_concept_with_always_false(bool) {
  return false;
}
inline constexpr bool ignore_mapf_concept_with_always_true(bool) {
  return true;
}
inline constexpr bool use_mapf_concept(bool b) { return b; }

inline void dummy() {}

}

