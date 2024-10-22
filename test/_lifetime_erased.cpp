#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/data/has_no_meta/has_no_meta.h"
#include "../include/virtual_void/erased/lifetime/data.h"
#include "../include/virtual_void/erased/lifetime/observer.h"
#include "../include/virtual_void/erased/lifetime/shared_const.h"
#include "../include/virtual_void/erased/lifetime/unique.h"
#include "../include/virtual_void/erased/lifetime/value.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;

struct A {
    std::string s;
};

namespace {
TEST_CASE("erased/lifetime/data") {
  {
    auto u1 = std::make_unique<erased::concrete_data<int>>(1);
    REQUIRE(*erased::reconcrete_cast<int>(*u1) == 1);
  }
  {
    const auto u1 = std::make_unique<erased::concrete_data<int>>(1);
    REQUIRE(*erased::reconcrete_cast<int>(*u1) == 1);
  }
}
TEST_CASE("erased/lifetime/observer") {
  {
    static_assert(std::same_as<erased::select_observer_t<std::string>,
                               erased::mutable_observer<>>);
    static_assert(std::same_as<erased::select_observer_t<const std::string>,
                               erased::const_observer<>>);

    std::string s{"hallo"};
    auto mo = erased::mutable_observer<>(s);
    static_assert(std::same_as<erased::typed_observer<std::string>::conrete_t,
                               std::string>);
    static_assert(
        std::same_as<erased::typed_observer<std::string const>::conrete_t,
                     std::string const>);
    auto co = erased::const_observer<>(s);
    auto tmo = erased::typed_observer<std::string>(mo);
    static_assert(std::derived_from<decltype(tmo), erased::mutable_observer<>>);
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
    REQUIRE(*reconcrete_cast<std::string>(mo) == "world");
    // auto tmo2 = erased::typed_observer< std::string * >{ co }; // shall not
    // compile
  }
  {
    const std::string s{"hallo"};
    // auto mo = erased::mutable_observer( &s ); // shall not compile
    auto co = erased::const_observer<>(s);
    auto tco = erased::typed_observer<std::string const>(co);
    static_assert(std::derived_from<decltype(tco), erased::const_observer<>>);
    REQUIRE(*tco == "hallo");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "hallo");
  }
}
TEST_CASE("erased/lifetime/unique") {
  {
    auto u1 = erased::unique(1);
    REQUIRE(*erased::reconcrete_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased::unique(1);
    REQUIRE(*erased::reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased::unique(std::in_place_type<A>, "hallo");
    REQUIRE(*erased::reconcrete_cast<std::string>(u1) == "hallo");
  }
  {
    auto t1 = erased::typed_unique(1);
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
    auto u1 = erased::shared_const(1);
    REQUIRE(*erased::reconcrete_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased::shared_const(1);
    REQUIRE(*erased::reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased::shared_const(std::in_place_type<A>, "hallo");
    REQUIRE(*erased::reconcrete_cast<std::string>(u1) == "hallo");
  }
  {
    auto t1 = erased::typed_shared_const<int>(1);
    //*t1 = 2; // shall not compile!
    REQUIRE(*t1 == 1);
    // auto e1 = t1; // shall not compile!
    auto e1 = t1;
    REQUIRE(t1);  // !moved
    REQUIRE(e1.data());
    t1 = as<int>(std::move(e1));
    REQUIRE(!e1);  // !moved
    REQUIRE(*t1 == 1);
  }
}
TEST_CASE("erased/lifetime/value") {
  using namespace erased;
  {
    auto u1 = value(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = value(A{"hallo"});
    static_assert(std::same_as<decltype(u1), lifetime_handle<value_data_ptr>>);
    static_assert(!std::derived_from<std::decay_t<decltype(u1)>, lifetime_handle<lifetime_handle<value_data_ptr>>> &&
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
    //auto v1 = typed_value<std::string>(std::string("hallo"));
    auto v1 = typed_value<std::string>("hallo");
    static_assert( std::same_as<std::decay_t<decltype(*v1)>, std::string>);
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
    auto t1 = data_trait<value_data_ptr>{}(a);
    REQUIRE(*reconcrete_cast<std::string>(t1) == "hallo");
  }
  {
    struct x_t {
      std::string s_;
    };
    x_t a{"hallo"};
    auto t1 = data_trait<value_data_ptr>{}(a);
    REQUIRE(reconcrete_cast<x_t>(t1)->s_ == "hallo");
  }
}
TEST_CASE("erased/lifetime/convert") {}
}  // namespace
