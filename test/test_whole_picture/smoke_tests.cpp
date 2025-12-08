#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <ranges>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>

using namespace whole_picture;
using namespace whole_picture::architecture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace anyxx;

namespace {
namespace internal {
ANY_MEMBER_FWD(, shapes::picture, picture_author, std::string)
}
const architecture::size full_screen{80, 25};

}  // namespace

TEST_CASE("example 1 core") {
//  std::cout << get_meta_data<core::surface>().get_type_info().name() << "\n";
  std::cout << get_meta_data<whole_picture::core::shapes::picture>()
                   .get_type_info()
                   .name()
            << "\n";
  std::cout << members_count<whole_picture::core::shapes::picture>() << "\n";
}

TEST_CASE("example 2 core circle") {
  for (auto r : std::views::iota(1, 7)) {
    core::surface b{full_screen};
    auto c = make_circle({12, 12}, r);
    mutable_observed_surface s{b};
    c.draw(s);
    b.flush();
  }
}

TEST_CASE("example 3 core line") {
  {
    core::surface b{full_screen};
    auto l = make_line({0, 24}, {79, 0});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    core::surface b{full_screen};
    auto l = make_line({0, 0}, {0, 24});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    core::surface b{full_screen};
    auto l = make_line({0, 0}, {79, 0});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    core::surface b{full_screen};
    auto l = make_line({0, 0}, {79, 24});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
}

TEST_CASE("example 3 architecture picture") {
  {
    core::surface duck{"  __", ">(o )___", " ( ._> /", "  `---'"};
    core::surface b{full_screen};
    auto pic = make_picture({5, 5}, duck);
    mutable_observed_surface s{b};
    pic.draw(s);
    b.flush();

    REQUIRE(members_count<shapes::picture>() == 1);
    auto any_shape = shapes::make_picture({0, 0}, duck);
    core::shapes::picture const* duck_shape =
        anyxx::unerase_cast<shapes::picture>(any_shape);
    CHECK(!duck_shape->get(internal::picture_author));
    auto duck_shape_clone = *duck_shape;
    duck_shape_clone[internal::picture_author] = "Max";
    CHECK(duck_shape_clone[internal::picture_author] == "Max");
  }
  {
    core::surface love_cpp{
        "_____$$$$_________$$$$",
        "___$$$$$$$$_____$$$$$$$$",
        "_$$$$$$$$$$$$_$$$$$$$$$$$$        CCC",
        "$$$$$$$$$$$$$$$$$$$$$$$$$$$      C   C",
        "$$$$$$$$$$$$$$$$$$$$$$$$$$$     C         +    +",
        "_$$$$$$$$$$$$$$$$$$$$$$$$$      C        +++  +++",
        "__$$$$$$$$$$$$$$$$$$$$$$$       C         +    +",
        "____$$$$$$$$$$$$$$$$$$$          C   C",
        "_______$$$$$$$$$$$$$              CCC",
        "__________$$$$$$$",
        "____________$$$",
        "_____________$",
    };
    core::surface b{full_screen};
    auto pic = make_picture({5, 5}, love_cpp);
    mutable_observed_surface s{b};
    pic.draw(s);
    b.flush();
  }
}

ANY_MEMBER_IMPL(internal, shapes::picture, picture_author, std::string)
