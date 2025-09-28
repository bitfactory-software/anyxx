#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>
#include <virtual_void/data/unique_trait.hpp>

namespace virtual_void::data {
using unique = erased_unique_ptr<void>;

inline unique unique_nullptr() {
  return {nullptr, [](auto) {}};
}

};  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::unique>
    : unique_trait {};
}  // namespace virtual_void

namespace virtual_void::data {
static_assert(is_erased_data<unique>);
}  // namespace virtual_void::data::has_no_meta