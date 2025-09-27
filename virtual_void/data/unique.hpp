#pragma once

#include <virtual_void/data/decorated_data.hpp>
#include <virtual_void/data/erased_unique_ptr.hpp>

namespace virtual_void::data {
using unique = erased_unique_ptr<decoration_base>;

inline unique unique_nullptr() {
  return {nullptr, [](auto) {}};
}

};  // namespace virtual_void::data
