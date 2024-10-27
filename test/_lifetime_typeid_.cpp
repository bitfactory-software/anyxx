#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/data/has_type_info/observer.h"
#include "../include/virtual_void/erased/data/has_type_info/shared_const.h"
#include "../include/virtual_void/erased/data/has_type_info/unique.h"
#include "../include/virtual_void/erased/data/has_type_info/value.h"
#include "class_hierarchy_test_hierarchy.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::erased::data::has_type_info;
using namespace TestDomain;

struct A {
  std::string s;
};

namespace {

TEST_CASE("has_type_info/lifetime/observer") {
  std::string s{"hallo"};
  auto mo = mutable_observer(s);
  REQUIRE(mo.data() == &s);
  REQUIRE(*static_cast<std::string const*>(mo.data()) == "hallo");
  REQUIRE(mo.meta()->type_info() == &typeid(std::string));
  REQUIRE(*static_cast<std::string const*>(mo.data()) == "hallo");
  static_assert(
      std::derived_from<mutable_observer,
                        erased::virtual_void<
                            erased::data::observer_ptr<void*, meta>>>);
  REQUIRE(*reconcrete_cast<const std::string>(mo) == "hallo");
  static_assert(
      std::same_as<typed_mutable_observer<std::string>::value_t, std::string>);
  static_assert(std::same_as<typed_const_observer<std::string const>::value_t,
                             std::string const>);
  auto co = const_observer(s);
  REQUIRE(*reconcrete_cast<const std::string>(co) == "hallo");
  {
    REQUIRE(*reconcrete_cast<const std::string>(mo) == "hallo");
    auto tmo = as<std::string>(std::move(mo));
    static_assert(std::derived_from<decltype(tmo), mutable_observer>);
    REQUIRE(*tmo == "hallo");
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
  }
  {
    mo = mutable_observer(s);
    REQUIRE(*reconcrete_cast<const std::string>(mo) == "world");
    *reconcrete_cast<std::string>(mo) = "hallo";
    REQUIRE(s == "hallo");
    auto tmo = as<std::string>(mo);
    REQUIRE(*reconcrete_cast<std::string>(mo) == "hallo");
    REQUIRE(mo);
    static_assert(std::derived_from<decltype(tmo), mutable_observer>);
    REQUIRE(*tmo == "hallo");
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
    REQUIRE(*reconcrete_cast<std::string>(mo) == "world");
  }
  // auto tmo2 = typed_observer< std::string * >{ co }; // shall not
  // compile
}

TEST_CASE("has_type_info/lifetime/shared_const") {
  auto d = shared_const{D{"shared hallo"}};
  shared_const x = as<D const>(d);
  auto d1 = as<D const>(x);
  REQUIRE(d1->data == "shared hallo");
  REQUIRE(d1.meta()->type_info() == &typeid(D));
  static_assert(std::derived_from<D, A1>);
  typed_shared_const<A1> a0{*d1};
  auto a1 = as<A1>(d1);
  typed_shared_const<A1> a2{A1{"a2->OK"}};
  typed_shared_const<A1> a3{std::in_place, "a3 in_place->OK"};
  A1 a1_pur{"a1_pur"};
  typed_shared_const<A1> a4{a1_pur};
  auto& a1r = *a2;
  auto s1 = a1r.data;
  auto s = a2->data;
  REQUIRE(a2->data == "a2->OK");
  REQUIRE(a3->data == "a3 in_place->OK");
  REQUIRE(a4->data == "a1_pur");
}

TEST_CASE("has_type_info/lifetime/unique") {
  auto c1 = unique(std::in_place_type<C>, "unique c1");
  REQUIRE(erased::reconcrete_cast<C>(c1)->data == "unique c1");
  auto c2 = typed_unique<C>(std::in_place, "unique c2");
  REQUIRE(c2->data == "unique c2");
  auto c3 = typed_unique<C>(C{"unique c3"});
  REQUIRE(c3->data == "unique c3");
  auto c4 = std::move(c3);
  REQUIRE(c4->data == "unique c3");

  auto d1 = unique(std::in_place_type<D>, "unique hallo");
  unique x{std::move(d1)};
  auto d = as<D>(std::move(x));
  REQUIRE(d->data == "unique hallo");
}

TEST_CASE("has_type_info/lifetime/value") {
  {
    auto u1 = value(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = value(A{"hallo"});
    static_assert(
        std::same_as<decltype(u1), erased::virtual_void<value_DATA>>);
    static_assert(
        std::derived_from<std::decay_t<decltype(u1)>,
                          erased::virtual_void<value_DATA>> &&
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
    auto t1 = erased::virtual_void_trait<value_DATA>{}(a);
    REQUIRE(*reconcrete_cast<std::string>(t1) == "hallo");
  }
  {
    struct x_t {
      std::string s_;
    };
    x_t a{"hallo"};
    auto t1 = erased::virtual_void_trait<value_DATA>{}(a);
    REQUIRE(reconcrete_cast<x_t>(t1)->s_ == "hallo");
  }
}
}  // namespace
