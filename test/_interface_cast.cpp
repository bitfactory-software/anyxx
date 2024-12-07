#include <catch.hpp>
#include <iostream>
#include <virtual_void/data/has_type_info/observer.hpp>
#include <virtual_void/data/has_type_info/shared_const.hpp>
#include <virtual_void/data/has_type_info/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/cast.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data::has_type_info;

namespace test_query_interface {

struct X {
  double d_;
  std::string to_string() const { return std::to_string(d_); }
  double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

}  // namespace test_query_interface

using namespace test_query_interface;

namespace {

ERASED_INTERFACE(to_string_i, (INTERFACE_CONST_METHOD(std::string, to_string)))
ERASED_INTERFACE(get_value_i, (INTERFACE_CONST_METHOD(double, get_value)))
ERASED_INTERFACE_(set_value_i, get_value_i,
                  (INTERFACE_METHOD(void, set_value, double)))

}  // namespace

TEST_CASE("prototype") {
  {
    enable_copy_cast<get_value_i, X, const_observer>();
    enable_copy_cast<get_value_i, X, mutable_observer>();
    enable_copy_cast<get_value_i, X, shared_const>();
    enable_copy_cast<get_value_i, X, unique>();
    enable_copy_cast<get_value_i, X, unique, shared_const>();
    enable_copy_cast<set_value_i, X, unique, shared_const>();
    enable_copy_cast<to_string_i, X, const_observer, unique>();
    enable_copy_cast<set_value_i, X, mutable_observer, unique>();

    enable_move_cast<to_string_i, X, unique, unique>();
    enable_move_cast<get_value_i, X, shared_const, unique>();
    REQUIRE(find_move<shared_const, unique>.is_defined<X>());
    REQUIRE(
        move<X, shared_const, unique>.is_defined<get_value_i<shared_const>>());
    static_assert(
        std::same_as<decltype(move<X, shared_const, unique>)::result_t,
                     base<shared_const>>);
    seal_casts();

    move_factory_method<shared_const, unique> const& move_unique_to_shared =
        find_move<shared_const, unique>.construct<X>();
  }

  {
    X x{3.14};
    to_string_i<const_observer> to_string_i_co{x};
    REQUIRE(to_string_i_co.to_string() == "3.140000");
    auto base =
        copy<X, const_observer, const_observer>.construct<get_value_i<const_observer>>(
            get_virtual_void(to_string_i_co));
    auto i =
        interface::unchecked_v_table_cast<get_value_i<const_observer>>(base);
    REQUIRE(i.get_value() == 3.14);
    std::cout << "prototype: " << i.get_value() << "\n";

    get_value_i<const_observer> i1 =
        copy_cast<get_value_i<const_observer>>(to_string_i_co);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << "prototype i1: " << i1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1)) ==
            get_data(get_virtual_void(to_string_i_co)));
  }
  {
    X x{3.14};
    to_string_i<shared_const> i0{x};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "prototype shared_const i0: " << i0.to_string() << "\n";

    get_value_i<shared_const> i1 = copy_cast<get_value_i<shared_const>>(i0);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << "prototype shared_const i1: " << i1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1)) == get_data(get_virtual_void(i0)));

    get_value_i<unique> iu1 = copy_cast<get_value_i<unique>>(i0);
    REQUIRE(iu1.get_value() == 3.14);
    std::cout << "prototype shared_const/unique iu1: " << iu1.get_value()
              << "\n";
    REQUIRE(get_data(get_virtual_void(iu1)) != get_data(get_virtual_void(i0)));

    set_value_i<unique> sv0 = copy_cast<set_value_i<unique>>(i0);
    REQUIRE(get_data(get_virtual_void(sv0)) != get_data(get_virtual_void(i0)));
    REQUIRE(sv0.get_value() == 3.14);
    std::cout << "prototype shared_const/unique sv0: " << sv0.get_value()
              << "\n";
    sv0.set_value(sv0.get_value() * 2);
    REQUIRE(sv0.get_value() == 6.28);
    std::cout << "prototype shared_const/unique sv0 (*2): " << sv0.get_value()
              << "\n";
    auto esc = erased<shared_const>(X{.d_ = 42});
    auto esc_i = query_interface<get_value_i<shared_const>>(esc);
    REQUIRE(esc_i.get_value() == 42);
  }
  {
    X x{3.14};
    to_string_i<unique> i0{x};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "prototype unique i0: " << i0.to_string() << "\n";

    base<unique> i1b = copy_cast<get_value_i<unique>>(i0);
    auto i1{
        std::move(unchecked_v_table_cast<get_value_i<unique>>(std::move(i1b)))};

    REQUIRE(i1.get_value() == 3.14);
    std::cout << "prototype unique i1: " << i1.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1)) != get_data(get_virtual_void(i0)));

    auto i1c = copy_cast<get_value_i<unique>>(i0);

    REQUIRE(i1c.get_value() == 3.14);
    std::cout << "prototype unique i1c: " << i1c.get_value() << "\n";
    REQUIRE(get_data(get_virtual_void(i1c)) != get_data(get_virtual_void(i0)));

    auto i1d = move_cast<to_string_i<unique>>(std::move(i1c));
    REQUIRE(get_data(get_virtual_void(i1d)));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_virtual_void(i1c));  // moved!
#pragma warning(pop)
    REQUIRE(i1d.to_string() == "3.140000");

    auto i1e = move_cast<get_value_i<shared_const>>(std::move(i1d));
    REQUIRE(get_data(get_virtual_void(i1e)));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_virtual_void(i1d));  // moved!
#pragma warning(pop)
    REQUIRE(i1e.get_value() == 3.14);

    auto ui1 = erased<unique>(x);
    auto to_string_co = query_interface<to_string_i<const_observer>>(ui1);
    REQUIRE(to_string_co.to_string() == "3.140000");

    auto set_value_mo = query_interface<set_value_i<mutable_observer>>(ui1);
    REQUIRE(set_value_mo.get_value() == 3.14);
    set_value_mo.set_value(1.44);
    REQUIRE(to_string_co.to_string() == "1.440000");
    REQUIRE(unerase_cast<X>(&ui1)->get_value() == 1.44);
  }
}
