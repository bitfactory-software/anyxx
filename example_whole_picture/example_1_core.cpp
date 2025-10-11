#include <catch.hpp>
#include <example_whole_picture/layer_0_architecture/architecture.hpp>
#include <example_whole_picture/layer_0_architecture/picture.hpp>
#include <example_whole_picture/layer_1_core/shapes/circle/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <iostream>
#include <ranges>

using namespace Catch::Matchers;

using namespace whole_picture;
using namespace whole_picture::architecture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

VV_MEMBER_FWD(, shapes::picture, picture_author, std::string)

TEST_CASE("example 1 core") {
  std::cout << whole_picture::core::hello() << "\n";
  std::cout << runtime::get_meta_data<architecture::picture>().get_type_info().name() << "\n";
  std::cout << runtime::get_meta_data<whole_picture::core::shapes::picture>().get_type_info().name() << "\n";
  std::cout << open_object::members_count<whole_picture::core::shapes::picture>() << "\n";
}

TEST_CASE("example 2 core circle") {
  for (auto r : std::views::iota(1, 7)) {
    architecture::picture b{80, 25};
    auto c = make_circle({12, 12}, r);
    mutable_observed_surface s{b};
    c.draw(s);
    b.flush();
  }
}

TEST_CASE("example 3 core line") {
  {
    architecture::picture b{80, 25};
    auto l = make_line({0, 24}, {79, 0});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    architecture::picture b{80, 25};
    auto l = make_line({0, 0}, {0, 24});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    architecture::picture b{80, 25};
    auto l = make_line({0, 0}, {79, 0});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
  {
    architecture::picture b{80, 25};
    auto l = make_line({0, 0}, {79, 24});
    mutable_observed_surface s{b};
    l.draw(s);
    b.flush();
  }
}

TEST_CASE("example 3 architecture picture") {
  {
    architecture::picture duck{"  __", ">(o )___", " ( ._> /", "  `---'"};
    architecture::picture b{80, 25};
    auto pic = make_picture({5, 5}, duck);
    mutable_observed_surface s{b};
    pic.draw(s);
    b.flush();

    REQUIRE(open_object::members_count<shapes::picture>() == 1);
    auto any_shape = shapes::make_picture({0, 0}, duck);
    core::shapes::picture const* duck_shape =
        virtual_void::interface::unerase_cast<shapes::picture>(any_shape);
    CHECK(!duck_shape->get(picture_author));
    auto duck_shape_clone = *duck_shape;
    duck_shape_clone[picture_author] = "Max";
    CHECK(duck_shape_clone[picture_author] == "Max");
  }
  {
    architecture::picture love_cpp{
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
    architecture::picture b{80, 25};
    auto pic = make_picture({5, 5}, love_cpp);
    mutable_observed_surface s{b};
    pic.draw(s);
    b.flush();
  }
}

VV_MEMBER_IMPL(, shapes::picture, picture_author, std::string)
