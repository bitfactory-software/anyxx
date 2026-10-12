#pragma once

namespace anyxx {

using const_void = void const*;
using mutable_void = void*;
template <typename V>
concept voidness =
    (std::same_as<V, const_void> || std::same_as<V, mutable_void>);
template <voidness Voidness>
struct is_const_void_;
template <>
struct is_const_void_<void*> : std::false_type {};
template <>
struct is_const_void_<void const*> : std::true_type {};
template <typename Voidness>
concept is_const_void = is_const_void_<Voidness>::value;

}  // namespace anyxx