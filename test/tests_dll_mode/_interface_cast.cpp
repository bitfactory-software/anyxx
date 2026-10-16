#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "test/component_base/component_base.hpp"

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;
using namespace test::component_base;

namespace test::component_base {

TRAIT(unused_i, anyxx::dynamic_copyable, (ANY_FN(int, fun, (), const)))

}

TEST_CASE("_interface_cast") {
  {
    any<cref, to_string_i> to_string_i_co{
        test::component_base::get_to_string_i_co()};
    auto s = to_string_i_co.to_string();
    REQUIRE(s == "3.140000");

    static_assert(borrowable_from<any<cref, get_value_i>::proxy_t,
                                        any<cref, get_value_i>::proxy_t,
                                        any<cref, get_value_i>::v_table_t>);

    any<cref, get_value_i> i1 =
        *borrow_as<any<cref, get_value_i>>(to_string_i_co);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << " i1: " << i1.get_value() << "\n";
    REQUIRE(get_proxy_ptr(i1) == get_proxy_ptr(to_string_i_co));

    auto queried = borrow_as<any<cref, unused_i>>(to_string_i_co);
    CHECK(!queried.has_value());
    CHECK(std::string(queried.error().to.name()) ==
          std::string(typeid(unused_i_v_table).name()));
  }
  {
    any<shared, to_string_i> i0{test::component_base::get_to_string_i_sc(3.14)};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "shared i0: " << i0.to_string() << "\n";

    any<shared, get_value_i> i1 = *borrow_as<any<shared, get_value_i>>(i0);
    REQUIRE(i1.get_value() == 3.14);
    std::cout << "shared i1: " << i1.get_value() << "\n";
    REQUIRE(get_proxy_ptr(i1) == get_proxy_ptr(i0));

    any<unique, get_value_i> iu1 = *clone_to<any<unique, get_value_i>>(i0);
    REQUIRE(iu1.get_value() == 3.14);
    std::cout << "shared/unique iu1: " << iu1.get_value() << "\n";
    REQUIRE(get_proxy_ptr(iu1) != get_proxy_ptr(i0));

    any<unique, set_value_i> sv0 = *clone_to<any<unique, set_value_i>>(iu1);
    REQUIRE(get_proxy_ptr(sv0) != get_proxy_ptr(i0));
    REQUIRE(sv0.get_value() == 3.14);
    std::cout << "shared/unique sv0: " << sv0.get_value() << "\n";
    sv0.set_value(sv0.get_value() * 2);
    REQUIRE(sv0.get_value() == 6.28);
    std::cout << "shared/unique sv0 (*2): " << sv0.get_value() << "\n";
    {
      any<cref, get_value_i> get_value_i_const_observer{sv0};
      REQUIRE(get_value_i_const_observer.get_value() == 6.28);
      auto svc = downcast_to<any<cref, set_value_i>>(get_value_i_const_observer);
      CHECK(svc);
      // svc->set_value(666); // does not compile!
    }
    {
      any<mutref, get_value_i> get_value_i_const_observer{sv0};
      REQUIRE(get_value_i_const_observer.get_value() == 6.28);
      auto svc =
          downcast_to<any<mutref, set_value_i>>(get_value_i_const_observer);
      CHECK(svc);
      svc->set_value(666);               // NOLINT
      REQUIRE(svc->get_value() == 666);  // NOLINT
    }
    {
      static_assert(std::derived_from<any<unique, get_value_i>::v_table_t,
                                      any<unique, get_value_i>::v_table_t>);

      any<unique, get_value_i> get_value_i_unique{std::move(sv0)};
      REQUIRE(get_value_i_unique.get_value() == 666);
      auto svu =
          downcast_to<any<unique, set_value_i>>(std::move(get_value_i_unique));
      CHECK(svu);
      svu->set_value(1.44);             // NOLINT
      CHECK(svu->get_value() == 1.44);  // NOLINT
    }
  }
  {
    any<unique, to_string_i> i0{test::component_base::get_to_string_i_u(3.14)};
    REQUIRE(i0.to_string() == "3.140000");
    std::cout << "unique i0: " << i0.to_string() << "\n";

    auto i1b = *clone_to<any<unique, get_value_i>>(i0);
    auto i1{unchecked_downcast_to<any<unique, get_value_i>>(std::move(i1b))};

    REQUIRE(i1.get_value() == 3.14);
    std::cout << "unique i1: " << i1.get_value() << "\n";
    REQUIRE(get_proxy_ptr(i1) != get_proxy_ptr(i0));

    auto i1c = clone_to<any<unique, get_value_i>>(i0);

    REQUIRE(i1c->get_value() == 3.14);
    std::cout << "unique i1c: " << i1c->get_value() << "\n";
    REQUIRE(get_proxy_ptr(*i1c) != get_proxy_ptr(i0));

    auto i1d = move_to<any<unique, to_string_i>>(std::move(*i1c));
    REQUIRE(get_proxy_ptr((i1d)));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_proxy(*i1c));  // moved!
#pragma warning(pop)
    REQUIRE(i1d.to_string() == "3.140000");

    auto i1e = move_to<any<shared, get_value_i>>(std::move(i1d));
    REQUIRE(get_proxy_ptr(i1e));
#pragma warning(push)
#pragma warning(disable : 26800)
    REQUIRE(!get_proxy(i1d));  // NOLINT
#pragma warning(pop)
    REQUIRE(i1e.get_value() == 3.14);
  }
}
