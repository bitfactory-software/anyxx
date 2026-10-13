#pragma once

#include <bit_factory/anyxx/any_concept.hpp>

namespace anyxx {

template <is_any Any>
inline auto& get_proxy(Any const& any) {
  return any.proxy_;
}
template <is_any Any>
inline auto& get_proxy(Any& any) {
  return any.proxy_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::is_dyn)
inline auto& get_proxy_value(Any&& any) {
  return get_proxy(std::forward<Any>(any)).value_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::is_dyn)
inline auto const& get_proxy_value(Any const& any) {
  return get_proxy(any).value_;
}
template <is_any Any>
inline decltype(auto) move_proxy(Any&& any) {
  return std::move(any.proxy_);
}
template <is_any Any>
inline auto get_proxy_ptr_const(Any const& any) {
  return get_proxy_ptr(get_proxy(any), get_v_table(any));
}
template <is_any Any>
inline auto get_proxy_ptr(Any&& any) {
  return get_proxy_ptr(get_proxy(std::forward<Any>(any)), get_v_table(any));
}

template <typename To>
  requires(!is_any<To>)
auto unchecked_v_table_downcast_to(auto* v_table) {
  return static_cast<To*>(v_table);
}
template <typename To>
  requires is_any<To>
auto unchecked_v_table_downcast_to(auto* v_table) {
  return unchecked_v_table_downcast_to<typename To::v_table_t>(v_table);
}

template <typename Any>
  requires is_any<Any> && Any::is_dyn
inline auto get_v_table(Any const& any) {
  return unchecked_v_table_downcast_to<Any>(any.get_v_table_ptr());
}

template <is_any To, is_any From>
inline To unchecked_downcast_to(From from)
  requires(is_any_derived_from<To, From>)
{
  return To{std::move(from.proxy_),
            unchecked_v_table_downcast_to<To>(get_v_table(from))};
}

template <typename Trait, is_proxy Proxy>
inline auto release_v_table(any<Trait, Proxy>& from) {
  return from.release_v_table();
}

/// \defgroup casts Casts
/// \brief Mix and match downcast, crosscast, and obtain \ref any with other
///
/// IMPORTANT: For crosscasts to work, the models must be registered with the
/// \ref ANY_REGISTER_MODEL macro.

/// \brief Unsafe downcast to an unerased type
/// \ingroup casts
template <typename U, is_any Any>
inline auto unchecked_unerase_cast(Any const& o) {
  return unchecked_unerase_cast<U>(get_proxy(o), get_v_table(o));
}

}  // namespace anyxx