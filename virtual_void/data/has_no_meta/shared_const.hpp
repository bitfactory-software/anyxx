#pragma once

#include <virtual_void/data/shared_const_trait.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/has_no_meta/meta.hpp>

namespace virtual_void::data::has_no_meta {
using shared_const = data::shared_const<meta>;
};

namespace virtual_void {
template <>
struct erased_data_trait<data::has_no_meta::shared_const>
    : shared_const_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
template <typename T>
using typed_shared_const = virtual_typed<T const, shared_const>;
static_assert(is_virtual_void<shared_const>);
static_assert(is_virtual_typed<typed_shared_const<int> >);
}  // namespace virtual_void::data::has_no_meta