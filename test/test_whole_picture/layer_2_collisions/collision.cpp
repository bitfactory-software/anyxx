#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_2_collisions/collision.hpp>

using namespace whole_picture::collision;

happened_dispatch_t& whole_picture::collision::happened() {
    static happened_dispatch_t dispatch;
  return dispatch;
}


