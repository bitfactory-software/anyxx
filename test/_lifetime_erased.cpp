#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/data/has_no_meta/meta.h"
#include "../include/virtual_void/erased/data/has_no_meta/observer.h"
#include "../include/virtual_void/erased/data/has_no_meta/shared_const.h"
#include "../include/virtual_void/erased/data/has_no_meta/unique.h"
#include "../include/virtual_void/erased/data/has_no_meta/value.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void::erased::data::has_no_meta;

struct A {
  std::string s;
};

namespace {
TEST_CASE("erased/lifetime/observer") {
  {
    std::string s{"hallo"};
    auto mo = mutable_observer(s);
    static_assert(
        std::same_as<typed_const_observer<std::string>::value_t,
                     std::string>);
    static_assert(
        std::same_as<typed_mutable_observer<std::string const>::value_t,
                     std::string const>);
    auto co = const_observer(s);
    auto tmo = as<std::string>(mo);
    static_assert(std::derived_from<decltype(tmo), mutable_observer>);
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
    REQUIRE(*reconcrete_cast<std::string>(mo) == "world");
    // tmo = as< std::string >( co ); // shall not compile!
    auto tmo2 = as<std::string>(co);
    static_assert(decltype(tmo2)::is_const);
  }
  {
    const std::string s{"hallo"};
    //auto mo = mutable_observer(s);  // shall not compile
    auto co = const_observer(s);
    static_assert(std::is_const_v<std::remove_reference_t<std::string const>> ==
                  true);
    static_assert(typed_observer<std::string const, void*>::is_const ==
                  false);
    auto tco = as<std::string const>(co);
    static_assert(std::derived_from<decltype(tco), const_observer>);
    REQUIRE(*tco == "hallo");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "hallo");
  }
}
TEST_CASE("erased/lifetime/unique") {
  {
    auto u1 = unique(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    const auto u1 = unique(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = unique(std::in_place_type<A>, "hallo");
    REQUIRE(*reconcrete_cast<std::string>(u1) == "hallo");
  }
  {
    auto t1 = typed_unique<int>(1);
    static_assert( !t1.is_const );
    static_assert( std::same_as< decltype(t1)::value_t, int > );
    *t1 = 2;
    REQUIRE(*t1 == 2);
    // auto e1 = t1; // shall not compile!
    auto e1 = std::move(t1);
    REQUIRE(!t1);  // moved
    REQUIRE(e1.data());
    t1 = as<int>(std::move(e1));
    REQUIRE(!e1);  // moved
    REQUIRE(*t1 == 2);
  }
}
TEST_CASE("erased/lifetime/shared_const") {
  {
    auto u1 = shared_const(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    const auto u1 = shared_const(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = shared_const(std::in_place_type<A>, "hallo");
    REQUIRE(*reconcrete_cast<std::string>(u1) == "hallo");
  }
  {
    auto t1 = typed_shared_const<int>(1);
    //*t1 = 2; // shall not compile!
    REQUIRE(*t1 == 1);
    auto e1 = t1;
    REQUIRE(t1);  // !moved
    REQUIRE(e1.data());
    t1 = as<int const>(std::move(e1));
    REQUIRE(!e1);  // !moved
    REQUIRE(*t1 == 1);
  }
}
TEST_CASE("erased/lifetime/value") {
  {
    auto u1 = value(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = value(A{"hallo"});
    static_assert(
        std::same_as<decltype(u1), virtual_void::erased::virtual_void<value_data>>);
    static_assert(
        std::derived_from<std::decay_t<decltype(u1)>,
                          virtual_void::erased::virtual_void<value_data>> &&
        !std::same_as<std::decay_t<std::remove_pointer_t<decltype(u1)>>, void>);
    auto& u1cr = u1;
    auto a = reconcrete_cast<A>(u1);
    REQUIRE(a->s == "hallo");
    auto a1 = reconcrete_cast<A>(u1cr);
    REQUIRE(a1->s == "hallo");
    REQUIRE(reconcrete_cast<A>(u1)->s == "hallo");
    auto u2 = u1;
    REQUIRE(reconcrete_cast<A>(u1)->s.data() !=
            reconcrete_cast<A>(u2)->s.data());
    REQUIRE(reconcrete_cast<A>(u2)->s == "hallo");
    reconcrete_cast<A>(u2)->s = "world";
    REQUIRE(reconcrete_cast<A>(u1)->s == "hallo");
    REQUIRE(reconcrete_cast<A>(u2)->s == "world");
  }
  {
    auto v1 = typed_value<std::string>("hallo");
    static_assert(std::same_as<std::decay_t<decltype(*v1)>, std::string>);
    REQUIRE(*v1 == std::string{"hallo"});
    auto v2 = std::move(v1);
    REQUIRE(!v1);
    REQUIRE(*v2 == "hallo");
    v1 = std::move(v2);
    REQUIRE(!v2);
    REQUIRE(*v1 == "hallo");
  }
  {
    auto t1 = typed_value<int>(1);
    *t1 = 2;
    REQUIRE(*t1 == 2);
    auto e1 = t1;
    REQUIRE(t1);  // !moved
    REQUIRE(e1.data());
    t1 = as<int>(std::move(e1));
    REQUIRE(!e1);  // !moved
    REQUIRE(*t1 == 2);
  }
  {
    auto t1 = typed_value<std::string>("hallo");
    REQUIRE(*t1 == "hallo");
  }
  {
    std::string a = "hallo";
    auto t1 = value(a);
    REQUIRE(*reconcrete_cast<std::string>(t1) == "hallo");
  }
  {
    struct x_t {
      std::string s_;
    };
    x_t a{"hallo"};
    auto t1 = value(a);
    REQUIRE(reconcrete_cast<x_t>(t1)->s_ == "hallo");
  }
}
TEST_CASE("erased/lifetime/convert") {}
}  // namespace
