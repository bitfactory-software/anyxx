#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/crosscast_runtime.hpp>
#include <bit_factory/anyxx/proxies/dyn/cloneable_to.hpp>
#include <expected>

namespace anyxx {

/// \brief Clone via runtime information.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
std::expected<ToAny, cast_error> clone_to(FromAny const& from) {
  using vv_to_t = typename ToAny::proxy_t;
  static_assert(is_proxy<vv_to_t>);
  return query_v_table<ToAny>(get_v_table(from)).transform([&](auto v_table) {
    return ToAny{clone_to<vv_to_t>(get_proxy(from), v_table), v_table};
  });
}

}  // namespace anyxx