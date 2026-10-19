#pragma once

#include <bit_factory/anyxx/config.hpp>

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/any_forward.hpp>

namespace anyxx {

template <is_any Any>
inline auto& get_proxy(Any const& any_) {
  return any_.proxy_;
}
template <is_any Any>
inline auto& get_proxy(Any& any_) {
  return any_.proxy_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::is_dyn)
inline auto& get_proxy_value(Any&& any_) {
  return get_proxy(std::forward<Any>(any_)).value_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::is_dyn)
inline auto const& get_proxy_value(Any const& any_) {
  return get_proxy(any_).value_;
}
template <is_any Any>
inline decltype(auto) move_proxy(Any&& any_) {
  return std::move(any_.proxy_);
}
template <is_any Any>
inline auto get_proxy_ptr_const(Any const& any_) {
  return get_proxy_ptr(get_proxy(any_), get_v_table(any_));
}
template <is_any Any>
inline auto get_proxy_ptr(Any&& any_) {
  return get_proxy_ptr(get_proxy(std::forward<Any>(any_)), get_v_table(any_));
}

template <typename To>
  requires(!is_any<To>)
auto unchecked_v_table_downcast_to(auto* v_table) {
    auto void_p = static_cast<void*>(v_table);
    return static_cast<To*>(void_p);
}
template <typename To>
  requires is_any<To>
auto unchecked_v_table_downcast_to(auto* v_table) {
  return unchecked_v_table_downcast_to<typename To::v_table_t>(v_table);
}

template <typename Any>
  requires is_any<Any> && Any::is_dyn
inline auto get_v_table(Any const& any_) {
  return unchecked_v_table_downcast_to<Any>(any_.get_v_table_ptr());
}

template <is_any To, is_any From>
inline To unchecked_downcast_to(From from)
  requires(is_any_derived_from<To, From>)
{
  return To{std::move(from.proxy_),
            unchecked_v_table_downcast_to<To>(get_v_table(from))};
}

/// \defgroup casts Casts
/// \brief Mix and match downcast, crosscast, and obtain \ref any_ with other
///
/// IMPORTANT: For crosscasts to work, the models must be registered with the
/// \ref ANY_REGISTER_MODEL macro.

/// \brief Unsafe downcast to an unerased type
/// \ingroup casts
template <typename U, is_any Any>
inline decltype(auto) unchecked_any_cast(Any const& o) {
  return unchecked_proxy_cast<U>(get_proxy(o), get_v_table(o));
}

}  // namespace anyxx