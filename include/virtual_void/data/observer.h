#pragma once

#include "decorated_ptr.h"

namespace virtual_void::data {

template <typename VOID, typename META>
using observer = decorated_ptr<VOID, META>;

template <typename META>
using const_observer = observer<void const*, META>;
template <typename META>
using mutable_observer = observer<void*, META>;

}  // namespace virtual_void::data