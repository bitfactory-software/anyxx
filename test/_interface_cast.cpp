#include <catch.hpp>
#include <iostream>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/cast.hpp>

#include "./component_base/component_base.hpp"

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;

using namespace test::component_base;

namespace test::component_base {

VV_INTERFACE(unused_i, (VV_CONST_METHOD(int, fun)))

}

TEST_CASE("_interface_cast") {
  {
    to_string_i<const_observer> to_string_i_co{
        test::component_base::get_to_string_i_co()};
    auto s = to_string_i_co.to_string();
    REQUIRE(s == "3.140000");

    get_value_i<const_observer> i1 =
        *query_interface<get_value_i<const_observer>>(to_string_i_co);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << " i1: " << i1.get_value() << "\n";
    REQUIRE(get_void_data_ptr(i1) == get_void_data_ptr(to_string_i_co));

    auto queried =
        query_interface<unused_i<const_observer>>(to_string_i_co);
    CHECK(!queried.has_value());
    CHECK(std::string(queried.error().to.name()) ==
          std::string(typeid(unused_i_v_table).name()));
  }
  {
    to_string_i<shared_const> i0{
        test::component_base::get_to_string_i_sc(3.14)};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "shared_const i0: " << i0.to_string() << "\n";

    get_value_i<shared_const> i1 =
        *query_interface<get_value_i<shared_const>>(i0);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << "shared_const i1: " << i1.get_value() << "\n";
    REQUIRE(get_void_data_ptr(i1) == get_void_data_ptr(i0));

    get_value_i<unique> iu1 = *dynamic_interface_clone_cast<get_value_i<unique>>(i0);
    REQUIRE(iu1.get_value() == 3.14);
    std::cout << "shared_const/unique iu1: " << iu1.get_value() << "\n";
    REQUIRE(get_void_data_ptr(iu1) != get_void_data_ptr(i0));

    set_value_i<unique> sv0 = *dynamic_interface_clone_cast<set_value_i<unique>>(iu1);
    REQUIRE(get_void_data_ptr(sv0) != get_void_data_ptr(i0));
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

    base<unique> i1b = *dynamic_interface_clone_cast<get_value_i<unique>>(i0);
    auto i1{
        std::move(unchecked_v_table_cast<get_value_i<unique>>(std::move(i1b)))};

    REQUIRE(i1.get_value() == 3.14);
    std::cout << "unique i1: " << i1.get_value() << "\n";
    REQUIRE(get_void_data_ptr(i1) != get_void_data_ptr(i0));

    auto i1c = dynamic_interface_clone_cast<get_value_i<unique>>(i0);

    REQUIRE(i1c->get_value() == 3.14);
    std::cout << "unique i1c: " << i1c->get_value() << "\n";
    REQUIRE(get_void_data_ptr(*i1c) != get_void_data_ptr(i0));

    auto i1d = move_to_interface<to_string_i<unique>>(std::move(*i1c));
    REQUIRE(get_void_data_ptr((i1d)));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_erased_data(*i1c));  // moved!
#pragma warning(pop)
    REQUIRE(i1d.to_string() == "3.140000");

    auto i1e = move_to_interface<get_value_i<shared_const>>(std::move(i1d));
    REQUIRE(get_void_data_ptr(i1e));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_erased_data(i1d));  // moved!
#pragma warning(pop)
    REQUIRE(i1e.get_value() == 3.14);
  }
}
