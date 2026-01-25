#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/line_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/collision.hpp>

using namespace whole_picture;
using namespace whole_picture::architecture;
using namespace anyxx;

namespace {
const architecture::size screen{10, 10};
const core::surface cross{
    " X",
    "XXX",
    " X",
};

void draw_scene(std::vector<architecture::any_shape<cref>> const& shapes) {
  core::surface b{screen};
  architecture::any_surface<mutref> s{b};
  for (auto const& shape : shapes) shape.draw(s);
  std::cout << "0123456789\n";
  b.flush();
}

auto collision_happened(any_shape<cref> const& l, any_shape<cref> const& r) {
  return collision::happened(l, r);
}

}  // namespace

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

  CHECK(collision_happened(l1,l2));
  CHECK(collision_happened(l2, l1));
  CHECK(!collision_happened(l3, l4));
  CHECK(!collision_happened(l4, l3));
  CHECK(collision_happened(l1, l3));
  CHECK(collision_happened(l3, l1));
  CHECK(collision_happened(l1, l4));
  CHECK(collision_happened(l4, l1));
  CHECK(collision_happened(l1, l5));
  CHECK(collision_happened(l5, l1));
  CHECK(collision_happened(l1, l6));
  CHECK(collision_happened(l6, l1));
  CHECK(collision_happened(l1, l7));
  CHECK(collision_happened(l7, l1));
  CHECK(!collision_happened(l1, l8));
  CHECK(!collision_happened(l8, l1));
}

TEST_CASE("test collision picture intersect") {
  {
    core::shapes::picture p1{architecture::point{0, 0}, cross};
    {
      core::shapes::picture p2{architecture::point{0, 0}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
      CHECK(collision::pictures::intersect(p2, p1));
      CHECK(collision_happened(p1, p2));
      CHECK(collision_happened(p2, p1));
    }
    {
      core::shapes::picture p2{architecture::point{1, 1}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
      CHECK(collision::pictures::intersect(p2, p1));
      CHECK(collision_happened(p1, p2));
      CHECK(collision_happened(p2, p1));
    }
    {
      core::shapes::picture p2{architecture::point{2, 2}, cross};
      CHECK(!collision::pictures::intersect(p1, p2));
      CHECK(!collision::pictures::intersect(p2, p1));
      CHECK(!collision_happened(p1, p2));
      CHECK(!collision_happened(p2, p1));
    }
  }
  {
    core::shapes::picture p1{architecture::point{2, 2}, cross};
    {
      core::shapes::picture p2{architecture::point{0, 0}, cross};
      CHECK(!collision::pictures::intersect(p1, p2));
      CHECK(!collision::pictures::intersect(p2, p1));
      CHECK(!collision_happened(p1, p2));
      CHECK(!collision_happened(p2, p1));
    }
    {
      core::shapes::picture p2{architecture::point{1, 1}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
      CHECK(collision::pictures::intersect(p2, p1));
      CHECK(collision_happened(p1, p2));
      CHECK(collision_happened(p2, p1));
    }
    {
      core::shapes::picture p2{architecture::point{2, 2}, cross};
      CHECK(collision::pictures::intersect(p1, p2));
      CHECK(collision::pictures::intersect(p2, p1));
      CHECK(collision_happened(p1, p2));
      CHECK(collision_happened(p2, p1));
    }
  }
}

TEST_CASE("test collision line cross with fallback1") {
  core::shapes::line l{architecture::point{0, 0}, architecture::point{2, 2}};
  {
    core::shapes::picture p{architecture::point{0, 0}, cross};
    draw_scene({l, p});
    CHECK(collision::fallback::intersect(l, p));
    CHECK(collision::fallback::intersect(p, l));
    CHECK(collision_happened(l, p));
    CHECK(collision_happened(p, l));
  }
  {
    core::shapes::picture p{architecture::point{0, 1}, cross};
    draw_scene({l, p});
    CHECK(collision::fallback::intersect(l, p));
    CHECK(collision::fallback::intersect(p, l));
    CHECK(collision_happened(l, p));
    CHECK(collision_happened(p, l));
  }
  {
    core::shapes::picture p{architecture::point{0, 2}, cross};
    draw_scene({l, p});
    CHECK(!collision::fallback::intersect(l, p));
    CHECK(!collision::fallback::intersect(p, l));
    CHECK(!collision_happened(l, p));
    CHECK(!collision_happened(p, l));
  }
}

TEST_CASE("test collision line cross with fallback2") {
  core::shapes::picture p{architecture::point{0, 0}, cross};
  {
    core::shapes::line l{architecture::point{0, 0}, architecture::point{2, 2}};
    draw_scene({l, p});
    CHECK(collision::fallback::intersect(l, p));
    CHECK(collision::fallback::intersect(p, l));
    CHECK(collision_happened(l, p));
    CHECK(collision_happened(p, l));
  }
  {
    core::shapes::line l{architecture::point{0, 1}, architecture::point{2, 3}};
    draw_scene({l, p});
    CHECK(collision::fallback::intersect(l, p));
    CHECK(collision::fallback::intersect(p, l));
    CHECK(collision_happened(l, p));
    CHECK(collision_happened(p, l));
  }
  {
    core::shapes::line l{architecture::point{0, 2}, architecture::point{2, 4}};
    draw_scene({p, l});
    CHECK(!collision::fallback::intersect(l, p));
    CHECK(!collision::fallback::intersect(p, l));
    CHECK(!collision_happened(l, p));
    CHECK(!collision_happened(p, l));
  }
}

TEST_CASE("test collision line circle") {
  {
    core::shapes::line l{architecture::point{0, 0}, architecture::point{4, 4}};
    {
      core::shapes::circle c{.center = architecture::point{0, 0}, .radius = 4};
      draw_scene({l, c});
      CHECK(collision::fallback::intersect(l, c));
      CHECK(collision::fallback::intersect(c, l));
      CHECK(collision_happened(l, c));
      CHECK(collision_happened(c, l));
    }
    {
      core::shapes::circle c{.center = architecture::point{0, 1}, .radius = 4};
      draw_scene({l, c});
      CHECK(!collision::fallback::intersect(l, c));
      CHECK(!collision::fallback::intersect(c, l));
      CHECK(!collision_happened(l, c));
      CHECK(!collision_happened(c, l));
    }
    {
      core::shapes::circle c{.center = architecture::point{0, 2}, .radius = 4};
      draw_scene({l, c});
      CHECK(!collision::fallback::intersect(l, c));
      CHECK(!collision::fallback::intersect(c, l));
      CHECK(!collision_happened(l, c));
      CHECK(!collision_happened(c, l));
    }
    {
      core::shapes::circle c{.center = architecture::point{0, 3}, .radius = 4};
      draw_scene({l, c});
      CHECK(!collision::fallback::intersect(l, c));
      CHECK(!collision::fallback::intersect(c, l));
      CHECK(!collision_happened(l, c));
      CHECK(!collision_happened(c, l));
    }
    {
      core::shapes::circle c{.center = architecture::point{0, 4}, .radius = 4};
      draw_scene({l, c});
      CHECK(collision::fallback::intersect(l, c));
      CHECK(collision::fallback::intersect(c, l));
      CHECK(collision_happened(l, c));
      CHECK(collision_happened(c, l));
    }
    {
      core::shapes::circle c{.center = architecture::point{0, 5}, .radius = 4};
      draw_scene({l, c});
      CHECK(collision::fallback::intersect(l, c));
      CHECK(collision::fallback::intersect(c, l));
      CHECK(collision_happened(l, c));
      CHECK(collision_happened(c, l));
    }
  }
}
