#include <catch.hpp>
#include <iostream>
#include <virtual_void/data/has_type_info/observer.hpp>
#include <virtual_void/data/has_type_info/shared_const.hpp>
#include <virtual_void/data/has_type_info/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/conversion.hpp>

#include "./component_base/component_base.hpp"

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data::has_type_info;

using namespace test::component_base;

TEST_CASE("_interface_cast") {
  test::component_base::enable_casts();

  seal_casts();

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
    auto esc = test::component_base::sc_X(42);
    auto esc_i = attach_interface<get_value_i<shared_const>>(esc);
    REQUIRE(esc_i.get_value() == 42);
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

    unique ui1{ test::component_base::u_X(3.14) };
    auto to_string_co = attach_interface<to_string_i<const_observer>>(ui1);
    REQUIRE(to_string_co.to_string() == "3.140000");

    auto set_value_mo = attach_interface<set_value_i<mutable_observer>>(ui1);
    REQUIRE(set_value_mo.get_value() == 3.14);
    set_value_mo.set_value(1.44);
    REQUIRE(to_string_co.to_string() == "1.440000");

    auto ux11 = test::component_base::u_X(42.0);
    auto gv_i_sc = move_to_interface<get_value_i<shared_const>>(std::move(ux11));
    REQUIRE(gv_i_sc.get_value() == 42);
  }
}
