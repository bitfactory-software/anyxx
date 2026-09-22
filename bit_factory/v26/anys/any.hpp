#pragma once

#include <bit_factory/v26/dyn.hpp>

namespace anyxx26 {

using any_copyable = dyn<save_copyable>;
using any_moveable = dyn<save_moveable>;

}  // namespace

