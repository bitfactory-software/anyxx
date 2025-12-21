#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/line_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/collision.hpp>

using namespace whole_picture;
using line = core::shapes::line;
using circle = core::shapes::circle;
using picture = core::shapes::picture;

auto __ = collision::happened
              .define<architecture::shape<anyxx::const_observer>,
                      architecture::shape<anyxx::const_observer>>(
                  [](auto const& lhs, auto const& rhs) {
                    return collision::fallback::intersect(lhs, rhs);
                  });
auto __ = collision::happened.define<line, line>(
    [](auto const& lhs, auto const& rhs) {
      return collision::lines::intersect(lhs, rhs);
    });
auto __ = collision::happened.define<picture, picture>(
    [](auto const& lhs, auto const& rhs) {
      return collision::pictures::intersect(lhs, rhs);
    });

