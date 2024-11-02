#pragma once

#include "decorated_ptr.h"

namespace virtual_void::data {

template <typename VOID, typename META>
using observer = decorated_ptr<VOID, META>;

}  // namespace virtual_void::data