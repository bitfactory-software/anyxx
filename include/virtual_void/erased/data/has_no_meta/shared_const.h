#pragma once

#include "../shared_const_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_no_meta {
using shared_const_DATA = data::shared_const_ptr<base<meta>>;
};

namespace virtual_void::erased {
template <>
struct virtual_void_trait<data::has_no_meta::shared_const_DATA>
    : shared_const_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_no_meta {
using shared_const = virtual_void<shared_const_DATA>;
template <typename T>
using typed_shared_const = virtual_typed<T const, shared_const_DATA>;
static_assert(erased::is_virtual_void<shared_const>);
static_assert(erased::is_virtual_void<typed_shared_const<int> >);
}  // namespace virtual_void::erased::data::has_no_meta