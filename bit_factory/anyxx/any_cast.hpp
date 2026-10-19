#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/proxies/proxy_cast.hpp>

namespace anyxx {

/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, typename Any>
  requires is_any<Any> && Any::is_dyn
inline decltype(auto) any_cast(Any const& o) {
  return proxy_cast<U>(get_proxy(o), get_v_table(o));
}
/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, typename Any>
  requires is_any<Any> && Any::is_dyn
inline decltype(auto) any_cast_if(Any const& o) {
  return proxy_cast_if<U>(get_proxy(o), get_v_table(o));
}


}  // namespace anyxx