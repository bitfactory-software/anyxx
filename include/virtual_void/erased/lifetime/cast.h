#pragma once

#include <stdexcept>
#include <type_traits>

#include "observer.h"
#include "shared_const.h"
#include "unique.h"
#include "value.h"

namespace virtual_void::erased {
template <typename TO, typename FROM>
TO lifetime_cast(FROM const&);

template <>
const_observer lifetime_cast<const_observer>(shared_const const& shared_const) {
  return shared_const.data();
}
template <>
const_observer lifetime_cast<const_observer>(unique const& unique) {
  return unique.data();
}
template <>
const_observer lifetime_cast<const_observer>(value const& value) {
  return value.data();
}
}  // namespace virtual_void::erased