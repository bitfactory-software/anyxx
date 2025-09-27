#pragma once


#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/unique_trait.hpp>

namespace virtual_void::data::has_no_meta {
using unique = data::unique;
inline auto unique_nullptr() { return data::unique_nullptr(); }
}  // namespace virtual_void::data::has_no_meta

namespace virtual_void {
template <>
struct erased_data_trait<data::has_no_meta::unique>
    : unique_trait {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
static_assert(is_erased_data<unique>);
}  // namespace virtual_void::data::has_no_meta