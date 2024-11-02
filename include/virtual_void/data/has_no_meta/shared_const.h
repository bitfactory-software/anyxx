#pragma once

#include "../shared_const_trait.h"
#include "meta.h"

namespace virtual_void::data::has_no_meta {
using shared_const = erased_shared_const<decoration_base<meta>>;
};

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_no_meta::shared_const>
    : shared_const_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
template <typename T>
using typed_shared_const = virtual_typed<T const, shared_const>;
static_assert(is_virtual_void<shared_const>);
static_assert(is_virtual_typed<typed_shared_const<int> >);
}  // namespace virtual_void::data::has_no_meta