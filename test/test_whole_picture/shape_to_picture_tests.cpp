#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <bit_factory/anyxx.hpp>

static_assert(anyxx::is_in_dll_mode);

#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shape_to_picture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>

using namespace whole_picture;
using namespace anyxx;

namespace {
const architecture::size screen{10, 10};
const core::surface cross{
    " X",
    "XXX",
    " X",
};

void draw_scene(
    std::vector<architecture::any_shape<cref>> const& shapes) {
  core::surface b{screen};
  architecture::any_surface<mutref> s{b};
  for (auto const& shape : shapes) shape.draw(s);
  std::cout << "0123456789\n";
  b.flush();
}
}  // namespace

TEST_CASE("test shape_to_picture 1") {
  auto picture = core::shape_to_picture(core::shapes::picture{{0, 0}, cross});
  CHECK(picture.content.at(architecture::point{0, 0}) == ' ');
  CHECK(picture.content.at(architecture::point{1, 0}) == 'X');
  CHECK(picture.content.at(architecture::point{2, 0}) == 0);
  CHECK(picture.content.at(architecture::point{0, 1}) == 'X');
  CHECK(picture.content.at(architecture::point{1, 1}) == 'X');
  CHECK(picture.content.at(architecture::point{2, 1}) == 'X');
  CHECK(picture.content.at(architecture::point{0, 2}) == ' ');
  CHECK(picture.content.at(architecture::point{1, 2}) == 'X');
  CHECK(picture.content.at(architecture::point{2, 2}) == 0);
}

TEST_CASE("test shape_to_picture 2") {
  auto picture = core::shape_to_picture(core::shapes::picture{{-1, -1}, cross});
  CHECK(picture.content.at(architecture::point{0, 0}) == ' ');
  CHECK(picture.content.at(architecture::point{1, 0}) == 'X');
  CHECK(picture.content.at(architecture::point{2, 0}) == 0);
  CHECK(picture.content.at(architecture::point{0, 1}) == 'X');
  CHECK(picture.content.at(architecture::point{1, 1}) == 'X');
  CHECK(picture.content.at(architecture::point{2, 1}) == 'X');
  CHECK(picture.content.at(architecture::point{0, 2}) == ' ');
  CHECK(picture.content.at(architecture::point{1, 2}) == 'X');
  CHECK(picture.content.at(architecture::point{2, 2}) == 0);
}

TEST_CASE("test shape_to_picture 3") {
  core::shapes::line l1{architecture::point{0, 0}, architecture::point{2, 2}};
  auto picture = core::shape_to_picture(l1);
  CHECK(picture.content.at(architecture::point{0, 0}) == '*');
  CHECK(picture.content.at(architecture::point{1, 1}) == '*');
  CHECK(picture.content.at(architecture::point{1, 0}) == ' ');
  CHECK(picture.content.at(architecture::point{0, 1}) == ' ');
}

TEST_CASE("test shape_to_picture 4") {
  core::shapes::line l1{architecture::point{0, 2}, architecture::point{3, -1}};
  draw_scene({l1});
  auto picture = core::shape_to_picture(l1);
  draw_scene({picture});
  CHECK(picture.content.at(architecture::point{0, 0}) == ' ');
  CHECK(picture.content.at(architecture::point{0, 1}) == ' ');
  CHECK(picture.content.at(architecture::point{0, 2}) == '*');
  CHECK(picture.content.at(architecture::point{1, 0}) == ' ');
  CHECK(picture.content.at(architecture::point{1, 1}) == '*');
  CHECK(picture.content.at(architecture::point{1, 2}) == ' ');
  CHECK(picture.content.at(architecture::point{2, 0}) == '*');
  CHECK(picture.content.at(architecture::point{2, 1}) == ' ');
  CHECK(picture.content.at(architecture::point{2, 2}) == ' ');
}
