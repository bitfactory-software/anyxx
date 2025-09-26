#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const_ptr.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/data/has_meta_runtime/unique_ptr.hpp>
#include <virtual_void/data/has_meta_runtime/value.hpp>

#include "a.hpp"

using namespace Catch::Matchers;

#include "class_hierarchy_test_hierarchy.hpp"

using namespace virtual_void;
using namespace virtual_void::data::has_meta_runtime;
using namespace TestDomain;

namespace {
    struct x_t {
      std::string s_;
    };
}
VV_RUNTIME_STATIC(type_info,x_t)

namespace {

TEST_CASE("m_table/lifetime/observer") {
  std::string s{"hallo"};
  auto mo = erased<mutable_observer>(s);
  REQUIRE(get_data(mo) == &s);
  REQUIRE(*static_cast<std::string const*>(get_data(mo)) == "hallo");
  REQUIRE(get_meta(mo)->type_info() ==
          &virtual_void::meta::runtime<virtual_void::meta::type_info, std::string>());
  REQUIRE(*static_cast<std::string const*>(get_data(mo)) == "hallo");
  static_assert(std::derived_from<mutable_observer, observer<void*>>);
  REQUIRE(*unerase_cast<const std::string>(mo) == "hallo");
  auto co = erased<const_observer>(s);
  REQUIRE(*unerase_cast<const std::string>(co) == "hallo");
  {
    REQUIRE(*unerase_cast<const std::string>(mo) == "hallo");
    *unerase_cast<std::string>(mo) = "world";
    REQUIRE(*unerase_cast<const std::string>(mo) == "world");
    REQUIRE(s == "world");
  }
  {
    mo = erased<mutable_observer>(s);
    REQUIRE(*unerase_cast<const std::string>(mo) == "world");
    *unerase_cast<std::string>(mo) = "hallo";
    REQUIRE(s == "hallo");
  }
  // auto tmo2 = typed_observer< std::string * >{ co }; // shall not
  // compile
}

TEST_CASE("m_table/lifetime/shared_const") {
  auto x = erased_in_place<shared_const, D>("shared hallo");
  REQUIRE(unerase_cast<D>(x)->data == "shared hallo");
}

TEST_CASE("m_table/lifetime/unique") {
  auto c1 = erased_in_place<unique, C>("unique c1");
  REQUIRE(unerase_cast<C>(c1)->data == "unique c1");

  auto d1 = erased_in_place<unique, D>("unique hallo");
  unique x{std::move(d1)};
  REQUIRE(unerase_cast<D>(x)->data == "unique hallo");
}

TEST_CASE("m_table/lifetime/value") {
  {
    auto u1 = erased<value>(1);
    REQUIRE(*unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased<value>(A{"hallo"});
    static_assert(std::same_as<decltype(u1), value>);
    static_assert(
        std::derived_from<std::decay_t<decltype(u1)>, value> &&
        !std::same_as<std::decay_t<std::remove_pointer_t<decltype(u1)>>, void>);
    auto& u1cr = u1;
    auto a = unerase_cast<A>(u1);
    REQUIRE(a->s == "hallo");
    auto a1 = unerase_cast<A>(u1cr);
    REQUIRE(a1->s == "hallo");
    REQUIRE(unerase_cast<A>(u1)->s == "hallo");
    auto u2 = u1;
    REQUIRE(unerase_cast<A>(u1)->s.data() != unerase_cast<A>(u2)->s.data());
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
TEST_CASE("m_table/shared_const_ptr") {
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
TEST_CASE("m_table/unique_ptr") {
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
    REQUIRE(unerase_cast<A>(u1)->s == "hallo");
  }
}
}  // namespace
