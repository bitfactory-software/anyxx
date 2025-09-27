#pragma once

#include <virtual_void/data/decorated_ptr.hpp>

namespace virtual_void::data {

template <typename VV_VOID>
using observer = decorated_ptr<VV_VOID>;

using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

}  // namespace virtual_void::data