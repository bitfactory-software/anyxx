#pragma once

#include "observer.h"
#include "shared_const.h"
#include "unique.h"
#include "value.h"

namespace virtual_void::data::has_no_meta {
template <typename TO, typename FROM>
TO lifetime_cast(FROM const&);

template <>
const_observer lifetime_cast<const_observer>(shared_const const& shared_const) {
  return const_observer{ data::observer_ptr{shared_const.data(), meta{std::in_place_type<void> } } };
}
template <>
const_observer lifetime_cast<const_observer>(unique const& unique) {
  return const_observer{ data::observer_ptr{unique.data(), meta{std::in_place_type<void> } } };
}
template <>
const_observer lifetime_cast<const_observer>(value const& value) {
  return const_observer{ data::observer_ptr{value.data(), meta{std::in_place_type<void> } } };
}
}  // namespace virtual_void