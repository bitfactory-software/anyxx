#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/crosscast_runtime.hpp>
#include <bit_factory/anyxx/proxies/dyn/moveable_from.hpp>

namespace anyxx {

/// \brief Move ownership to another \ref any. Uses runtime information to
/// crosscast, if necessary.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
ToAny move_to(FromAny&& from) {
  auto to_v_table = query_v_table<ToAny>(release_v_table(from));
  return ToAny{move_proxy(std::move(from)), *to_v_table};
}

}  // namespace anyxx