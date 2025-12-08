#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <ranges>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/line_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>

using namespace whole_picture;

using namespace anyxx;

TEST_CASE("test collision line intersect") {
  core::shapes::line l1{architecture::point{0, 0}, architecture::point{2, 2}};
  core::shapes::line l2{architecture::point{0, 3}, architecture::point{2, 0}};
  core::shapes::line l3{architecture::point{0, 0}, architecture::point{2, 0}};
  core::shapes::line l4{architecture::point{0, 2}, architecture::point{2, 2}};
  core::shapes::line l5{architecture::point{0, 0}, architecture::point{0, 2}};
  core::shapes::line l6{architecture::point{2, 0}, architecture::point{2, 2}};
  core::shapes::line l7{architecture::point{1, 0}, architecture::point{1, 2}};
  core::shapes::line l8{architecture::point{0, 1}, architecture::point{2, 3}};
  CHECK(collision::lines::intersect(l1, l2));
  CHECK(collision::lines::intersect(l2, l1));
  CHECK(!collision::lines::intersect(l3, l4));
  CHECK(!collision::lines::intersect(l4, l3));
  CHECK(collision::lines::intersect(l1, l3));
  CHECK(collision::lines::intersect(l3, l1));
  CHECK(collision::lines::intersect(l1, l4));
  CHECK(collision::lines::intersect(l4, l1));
  CHECK(collision::lines::intersect(l1, l5));
  CHECK(collision::lines::intersect(l5, l1));
  CHECK(collision::lines::intersect(l1, l6));
  CHECK(collision::lines::intersect(l6, l1));
  CHECK(collision::lines::intersect(l1, l7));
  CHECK(collision::lines::intersect(l7, l1));
  CHECK(!collision::lines::intersect(l1, l8));
  CHECK(!collision::lines::intersect(l8, l1));
}

TEST_CASE("test collision picture intersect") {
  core::surface cross{
      " X",
      "XXX",
      " X",
  };
  {
    core::shapes::picture p1{architecture::point{0, 0}, cross};
    {
      core::shapes::picture p2{architecture::point{0, 0}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
    }
    {
      core::shapes::picture p2{architecture::point{1, 1}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
    }
    {
      core::shapes::picture p2{architecture::point{2, 2}, cross};
      CHECK(!collision::pictures::intersect(p1, p2));
    }
  }
  {
    core::shapes::picture p1{architecture::point{2, 2}, cross};
    {
      core::shapes::picture p2{architecture::point{0, 0}, cross};
      CHECK(!collision::pictures::intersect(p1, p2));
    }
    {
      core::shapes::picture p2{architecture::point{1, 1}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
    }
    {
      core::shapes::picture p2{architecture::point{2, 2}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
    }
  }
}
