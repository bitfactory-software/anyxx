#include <test/test_whole_picture/layer_2_collision/core_collisions.hpp>
#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_2_collision/collision.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/line_intersect.hpp>

using namespace whole_picture;
using line = whole_picture::core::shapes::line;
using circle = whole_picture::core::shapes::circle;
using picture = whole_picture::core::shapes::picture;

auto __ = whole_picture::collision::happened().define<circle, circle>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(lhs, rhs); });
auto __ = whole_picture::collision::happened().define<line, line>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(lhs, rhs); });
auto __ = whole_picture::collision::happened().define<picture, picture>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(lhs, rhs); });

auto __ = whole_picture::collision::happened().define<circle, line>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(lhs, rhs); });
auto __ = whole_picture::collision::happened().define<line, circle>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(rhs, lhs); });
auto __ = whole_picture::collision::happened().define<line, picture>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(lhs, rhs); });
auto __ = whole_picture::collision::happened().define<picture, line>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(rhs, lhs); });

auto __ = whole_picture::collision::happened().define<circle, picture>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(lhs, rhs); });
auto __ = whole_picture::collision::happened().define<picture, circle>(
    [](auto const& lhs, auto const& rhs) { return collision::happens(rhs, lhs); });


bool collision::happens(circle const& lhs, circle const& rhs) {
  return fallback::intersect(lhs, rhs);
}
bool collision::happens(circle const& lhs, line const& rhs) {
  return fallback::intersect(lhs, rhs);
}
bool collision::happens(circle const& lhs, picture const& rhs) {
  return fallback::intersect(lhs, rhs);
}
bool collision::happens(line const& lhs, line const& rhs) {
  return lines::intersect(lhs, rhs);
}
bool collision::happens(line const& lhs, picture const& rhs) {
  return fallback::intersect(lhs, rhs);
}
bool collision::happens(picture const& lhs, picture const& rhs) {
  return pictures::intersect(lhs, rhs);
}
