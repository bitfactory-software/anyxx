#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/crosscast_runtime.hpp>
#include <bit_factory/anyxx/proxies/dyn/borrowable_from.hpp>
#include <expected>

namespace anyxx {

template <is_any ToAny, is_proxy FromProxy, typename FromVTable>
  requires borrowable_from<typename ToAny::proxy_t, FromProxy, FromVTable>
std::expected<ToAny, cast_error> borrow_as(FromProxy const& from,
                                           FromVTable* from_v_table) {
  using to = typename ToAny::proxy_t;
  return query_v_table<ToAny>(from_v_table).transform([&](auto v_table) {
    return ToAny{borrow_proxy_as<to>(from, v_table), v_table};
  });
}

/// \brief Safe crosscast via runtime information to another \ref any_ without
/// changing the ownership.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
  requires borrowable_from<typename ToAny::proxy_t,
                                 typename FromAny::proxy_t,
                                 typename FromAny::v_table_t>
std::expected<ToAny, cast_error> borrow_as(FromAny const& from) {
  if constexpr (is_any_derived_from<FromAny, ToAny>) {
    return {ToAny{from}};
  } else if constexpr (is_any_derived_from<ToAny, FromAny>) {
    return *downcast_to<ToAny>(from);
  } else {
    return borrow_as<ToAny>(get_proxy(from), get_v_table(from));
  }
}

}  // namespace anyxx