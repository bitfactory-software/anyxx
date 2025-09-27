#include <catch.hpp>
#include <iostream>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/conversion.hpp>

#include "./component_base/component_base.hpp"

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;
using namespace virtual_void::data::has_no_meta;

using namespace test::component_base;

TEST_CASE("_interface_cast") {

  {
    to_string_i<const_observer> to_string_i_co{
        test::component_base::get_to_string_i_co()};
    auto s = to_string_i_co.to_string();
    REQUIRE(s == "3.140000");

    get_value_i<const_observer> i1 =
        attach_interface<get_value_i<const_observer>>(to_string_i_co);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << " i1: " << i1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1)) ==
            get_data(get_virtual_void(to_string_i_co)));
  }
  {
    to_string_i<shared_const> i0{
        test::component_base::get_to_string_i_sc(3.14)};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "shared_const i0: " << i0.to_string() << "\n";

    get_value_i<shared_const> i1 = attach_interface<get_value_i<shared_const>>(i0);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << "shared_const i1: " << i1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1)) == get_data(get_virtual_void(i0)));

    get_value_i<unique> iu1 = attach_interface<get_value_i<unique>>(i0);
    REQUIRE(iu1.get_value() == 3.14);
    std::cout << "shared_const/unique iu1: " << iu1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(iu1)) != get_data(get_virtual_void(i0)));

    set_value_i<unique> sv0 = attach_interface<set_value_i<unique>>(i0);
    REQUIRE(get_data(get_virtual_void(sv0)) != get_data(get_virtual_void(i0)));
    REQUIRE(sv0.get_value() == 3.14);
    std::cout << "shared_const/unique sv0: " << sv0.get_value() << "\n";
    sv0.set_value(sv0.get_value() * 2);
    REQUIRE(sv0.get_value() == 6.28);
    std::cout << "shared_const/unique sv0 (*2): " << sv0.get_value() << "\n";
  }
  {
    to_string_i<unique> i0{test::component_base::get_to_string_i_u(3.14)};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "unique i0: " << i0.to_string() << "\n";

    base<unique> i1b = attach_interface<get_value_i<unique>>(i0);
    auto i1{
        std::move(unchecked_v_table_cast<get_value_i<unique>>(std::move(i1b)))};

    REQUIRE(i1.get_value() == 3.14);
    std::cout << "unique i1: " << i1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1)) != get_data(get_virtual_void(i0)));

    auto i1c = attach_interface<get_value_i<unique>>(i0);

    REQUIRE(i1c.get_value() == 3.14);
    std::cout << "unique i1c: " << i1c.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1c)) != get_data(get_virtual_void(i0)));

    auto i1d = move_to_interface<to_string_i<unique>>(std::move(i1c));
    REQUIRE(get_data(get_virtual_void(i1d)));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_virtual_void(i1c));  // moved!
#pragma warning(pop)
    REQUIRE(i1d.to_string() == "3.140000");

    auto i1e = move_to_interface<get_value_i<shared_const>>(std::move(i1d));
    REQUIRE(get_data(get_virtual_void(i1e)));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_virtual_void(i1d));  // moved!
#pragma warning(pop)
    REQUIRE(i1e.get_value() == 3.14);
  }
}
