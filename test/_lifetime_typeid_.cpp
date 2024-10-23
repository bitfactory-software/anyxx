#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/typeid/lifetime/observer.h"
#include "class_hierarchy_test_hierarchy.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::typeid_;

namespace {

TEST_CASE("typeid_/lifetime/observer") {
  std::string s{"hallo"};
  auto mo = typeid_::mutable_observer(s);
  REQUIRE(mo.data() == &s);
  REQUIRE(*static_cast<std::string const*>(mo.data()) == "hallo");
  REQUIRE(mo.meta()->type_info() == &typeid(std::string));
  REQUIRE(*static_cast<std::string const*>(mo.data()) == "hallo");
  static_assert(
      std::derived_from<typeid_::mutable_observer,
                        erased::virtual_void<erased::data::observer_ptr<
                            void*, erased::data::has_type_info>>>);
  REQUIRE(*reconcrete_cast<const std::string>(mo) == "hallo");
  static_assert(
      std::same_as<typeid_::typed_mutable_observer<std::string>::value_t,
                   std::string>);
  static_assert(
      std::same_as<typeid_::typed_const_observer<std::string const>::value_t,
                   std::string const>);
  auto co = typeid_::const_observer(s);
  REQUIRE(*reconcrete_cast<const std::string>(co) == "hallo");
  {
    REQUIRE(*reconcrete_cast<const std::string>(mo) == "hallo");
    auto tmo = as<std::string>(std::move(mo));
    static_assert(std::derived_from<decltype(tmo), typeid_::mutable_observer>);
    REQUIRE(*tmo == "hallo");
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
  }
  {
    mo = typeid_::mutable_observer(s);
    REQUIRE(*reconcrete_cast<const std::string>(mo) == "world");
    *reconcrete_cast<std::string>(mo) = "hallo";
    REQUIRE(s == "hallo");
    auto tmo = as<std::string>(mo);
    REQUIRE(*reconcrete_cast<std::string>(mo) == "hallo");
    REQUIRE(mo);
    static_assert(std::derived_from<decltype(tmo), typeid_::mutable_observer>);
    REQUIRE(*tmo == "hallo");
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
    REQUIRE(*reconcrete_cast<std::string>(mo) == "world");
  }
  // auto tmo2 = typeid_::typed_observer< std::string * >{ co }; // shall not
  // compile
}

//TEST_CASE("m_table/lifetime/shared_const") {
//  auto d = make_shared_const<D>("shared hallo");
//  shared_const x = as<D>(d);
//  auto d1 = as<D>(x);
//  REQUIRE(d->data == "shared hallo");
//  REQUIRE(d.type() == typeid(D));
//  static_assert(std::derived_from<D, A1>);
//  typed_shared_const<A1> a1 = d1;
//  typed_shared_const<A1> a2 = A1{"a2->OK"};
//  typed_shared_const<A1> a3{std::in_place, "a3 in_place->OK"};
//  A1 a1_pur{"a1_pur"};
//  typed_shared_const<A1> a4{a1_pur};
//  auto& a1r = *a2;
//  auto s1 = a1r.data;
//  auto s = a2->data;
//  REQUIRE(a2->data == "a2->OK");
//  REQUIRE(a3->data == "a3 in_place->OK");
//  REQUIRE(a4->data == "a1_pur");
//}
//
//TEST_CASE("m_table/lifetime/unique") {
//  auto c1 = make_unique<C>("unique c1");
//  REQUIRE(c1->data == "unique c1");
//  auto c2 = typed_unique<C>(std::in_place, "unique c2");
//  REQUIRE(c2->data == "unique c2");
//  auto c3 = typed_unique<C>(C{"unique c3"});
//  REQUIRE(c3->data == "unique c3");
//  auto c4 = std::move(c3);
//  REQUIRE(c4->data == "unique c3");
//
//  auto d1 = make_unique<D>("unique hallo");
//  unique x{std::move(d1)};
//  auto d = as<D>(std::move(x));
//  REQUIRE(d->data == "unique hallo");
//}
//
}  // namespace
