#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "class_hierarchy_test_hierarchy.hpp"

#include <catch.hpp>

#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/data/has_meta_runtime/value.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const_ptr.hpp>
#include <virtual_void/data/has_meta_runtime/unique_ptr.hpp>

#include "a.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace ::virtual_void::data::has_meta_runtime;
using namespace TestDomain;

namespace {
    struct x_t {
      std::string s_;
    };
}
VV_RUNTIME_STATIC(type_info,x_t)

namespace {

TEST_CASE("has_type_info/lifetime/observer") {
  std::string s{"hallo"};
  auto mo = erased<mutable_observer>(s);
    mutable_observer o1;
 REQUIRE(!o1);
  o1 = mo;
  REQUIRE(o1);
  REQUIRE(get_data(mo) == &s);
  REQUIRE(get_data(o1) == &s);
  REQUIRE(*static_cast<std::string const*>(get_data(mo)) == "hallo");
  REQUIRE(&get_meta(mo)->type_info()->get_type_info() == &typeid(std::string));
  REQUIRE(*static_cast<std::string const*>(get_data(mo)) == "hallo");
  REQUIRE(*unerase_cast<const std::string>(mo) == "hallo");
  auto co = erased<const_observer>(s);
  REQUIRE(*unerase_cast<const std::string>(co) == "hallo");
  {
    mo = erased<mutable_observer>(s);
    *unerase_cast<std::string>(mo) = "world";
    REQUIRE(*unerase_cast<const std::string>(mo) == "world");
    REQUIRE(s == "world");
    *unerase_cast<std::string>(mo) = "hallo";
    REQUIRE(s == "hallo");
    REQUIRE(*unerase_cast<std::string>(mo) == "hallo");
    REQUIRE(has_data(mo));
  }
  // auto tmo2 = typed_observer< std::string * >{ co }; // shall not
  // compile
}

TEST_CASE("has_type_info/lifetime/shared_const") {
  auto d = erased_in_place<shared_const, D>("shared hallo");
  REQUIRE(unerase_cast<D>(d)->data == "shared hallo");
}

TEST_CASE("has_type_info/lifetime/unique") {
  auto c1 = erased_in_place<unique, C>("unique c1");
  REQUIRE(unerase_cast<C>(c1)->data == "unique c1");

  unique d1 = erased_in_place<unique, D>("unique hallo");
  static_assert(is_erased_data<unique>);
  unique x{std::move(d1)};
}

TEST_CASE("has_type_info/lifetime/value") {
  {
    auto u1 = erased<value>(1);
    REQUIRE(*unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased<value>(A{"hallo"});
    auto& u1cr = u1;
    auto a = unerase_cast<A>(u1);
    REQUIRE(a->s == "hallo");
    auto a1 = unerase_cast<A>(u1cr);
    REQUIRE(a1->s == "hallo");
    REQUIRE(unerase_cast<A>(u1)->s == "hallo");
    auto u2 { u1 };
    REQUIRE(unerase_cast<A>(u1)->s.data() !=
            unerase_cast<A>(u2)->s.data());
    REQUIRE(unerase_cast<A>(u2)->s == "hallo");
    unerase_cast<A>(u2)->s = "world";
    REQUIRE(unerase_cast<A>(u1)->s == "hallo");
    REQUIRE(unerase_cast<A>(u2)->s == "world");
  }
  {
    std::string a = "hallo";
    auto t1 = erased<value>(a);
    REQUIRE(*unerase_cast<std::string>(t1) == "hallo");
  }
  {
    x_t a{"hallo"};
    auto t1 = erased<value>(a);
    REQUIRE(unerase_cast<x_t>(t1)->s_ == "hallo");
  }
}
TEST_CASE("has_type_info/shared_const_ptr") {
  {
    auto ptr = std::make_shared<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    shared_const_ptr sp1 =
        erased_data_trait<shared_const_ptr>::construct_from(ptr);
    auto u1 = erased<shared_const_ptr>(ptr);
    A const* a = unerase_cast<A>(u1);
    REQUIRE(unerase_cast<A>(u1)->s == "hallo");
  }
}
TEST_CASE("has_type_info/unique_ptr") {
  {
    auto ptr = std::make_unique<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    unique_ptr up1 =
        erased_data_trait<unique_ptr>::construct_from(std::move(ptr));
  }
  {
    auto ptr = std::make_unique<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    auto u1 = erased<unique_ptr>(std::move(ptr));
    A const* a = unerase_cast<A>(u1);
    REQUIRE_THROWS_AS(unerase_cast<std::string>(u1), type_mismatch_error);
    REQUIRE(unerase_cast<A>(u1)->s == "hallo");
  }
}
}  // namespace
