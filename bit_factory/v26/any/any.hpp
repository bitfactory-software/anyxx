#include <bit_factory/v26/dyn.hpp>

namespace anyxx26 {

using any_copyable = dyn<save_copyable>;
using any_copy_refable = dyn<save_copyable, anyxx::cref>;
using any_moveable = dyn<save_moveable>;
using any_move_refable = dyn<save_moveable, anyxx::cref>;

}  // namespace

