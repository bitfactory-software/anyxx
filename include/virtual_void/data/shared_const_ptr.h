#pragma once

#include <memory>

#include "decorated_ptr.h"

namespace virtual_void::data {

template <typename VOID, typename META>
using shared_const_ptr = decorated_ptr<std::shared_ptr<void>, META>;

}  // namespace virtual_void::data