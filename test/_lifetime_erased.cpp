#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "a.h"
#include "include/catch.hpp"
#include "virtual_void/data/has_no_meta/meta.h"
#include "virtual_void/data/has_no_meta/observer.h"
#include "virtual_void/data/has_no_meta/shared_const.h"
#include "virtual_void/data/has_no_meta/shared_const_ptr.h"
#include "virtual_void/data/has_no_meta/unique.h"
#include "virtual_void/data/has_no_meta/unique_ptr.h"
#include "virtual_void/data/has_no_meta/value.h"

using namespace Catch::Matchers;

using namespace virtual_void::data::has_no_meta;
using namespace virtual_void;

namespace {
template <typename CONST_OBSERVER, typename TYPED_MUTABLE_STRING_OBSERVER>
constexpr bool compiles1 =
    requires(CONST_OBSERVER co, TYPED_MUTABLE_STRING_OBSERVER mo) {
      mo = as<std::string>(co);
    };

TEST_CASE("lifetime/observer") {
  {
    std::string s{"hallo"};
    auto mo = erased<mutable_observer>(s);
    static_assert(
        std::same_as<typed_const_observer<std::string>::value_t, std::string>);
    static_assert(
        std::same_as<typed_mutable_observer<std::string const>::value_t,
                     std::string const>);
    auto co = erased<const_observer>(s);
    auto tmo = as<std::string>(mo);
    *tmo = "world";
    REQUIRE(s == "world");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "world");
    REQUIRE(*reconcrete_cast<std::string>(mo) == "world");
    {
      const std::string cs;
      // mutable_observer xx(cs);  // shall not compile!
    }
    // tmo = as< std::string >( co ); // shall not compile!
    static_assert(
        !compiles1<const_observer, typed_mutable_observer<std::string>>);
    auto co2 = as<std::string>(co);
    static_assert(decltype(co2)::is_const);
  }
  {
    const std::string s{"hallo"};
    // auto mo = mutable_observer(s);  // shall not compile
    auto co = erased<const_observer>(s);
    static_assert(std::is_const_v<std::remove_reference_t<std::string const>> ==
                  true);
    static_assert(typed_observer<std::string const, void*>::is_const == false);
    auto tco = as<std::string const>(co);
    REQUIRE(*tco == "hallo");
    REQUIRE(*reconcrete_cast<const std::string>(co) == "hallo");
  }
}
TEST_CASE("lifetime/unique") {
  {
    auto u1 = erased<unique>(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased<unique>(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased_in_place<unique, A>("hallo");
    REQUIRE(reconcrete_cast<A>(u1)->s == "hallo");
  }
  {
    auto t1 = typed_unique<int>(1);
    static_assert(!t1.is_const);
    static_assert(std::same_as<decltype(t1)::value_t, int>);
    *t1 = 2;
    REQUIRE(*t1 == 2);
    // auto e1 = t1; // shall not compile!
    auto e1 = std::move(t1);
    REQUIRE(!has_data(t1));  // moved
    REQUIRE(get_data(e1));
    t1 = as<int>(std::move(e1));
    REQUIRE(!has_data(e1));  // moved
    REQUIRE(*t1 == 2);
  }
}
TEST_CASE("lifetime/shared_const") {
  {
    auto u1 = erased<shared_const>(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased<shared_const>(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased_in_place<shared_const, A>("hallo");
    REQUIRE(reconcrete_cast<A>(u1)->s == "hallo");
  }
  {
    auto t1 = typed_shared_const<int>(1);
    //*t1 = 2; // shall not compile!
    REQUIRE(*t1 == 1);
    auto e1 = t1;
    REQUIRE(has_data(t1));  // !moved
    REQUIRE(get_data(e1));
    t1 = as<int const>(std::move(e1));
    REQUIRE(!has_data(e1));  // !moved
    REQUIRE(*t1 == 1);
  }
}
TEST_CASE("lifetime/value") {
  {
    auto u1 = erased<value>(1);
    REQUIRE(*reconcrete_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased_in_place<value, A>("hallo");
    static_assert(std::same_as<decltype(u1), value>);
    static_assert(
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
    REQUIRE(!has_data(v1));
    REQUIRE(*v2 == "hallo");
    v1 = std::move(v2);
    REQUIRE(!has_data(v2));
    REQUIRE(*v1 == "hallo");
  }
  {
    auto t1 = typed_value<int>(1);
    *t1 = 2;
    REQUIRE(*t1 == 2);
    auto e1 = t1;
    REQUIRE(has_data(t1));  // !moved
    REQUIRE(get_data(e1));
    t1 = as<int>(std::move(e1));
    REQUIRE(!has_data(e1));  // !moved
    REQUIRE(*t1 == 2);
  }
  {
    auto t1 = typed_value<std::string>("hallo");
    REQUIRE(*t1 == "hallo");
  }
  {
    std::string a = "hallo";
    auto t1 = erased<value>(a);
    REQUIRE(*reconcrete_cast<std::string>(t1) == "hallo");
  }
  {
    struct x_t {
      std::string s_;
    };
    x_t a{"hallo"};
    auto t1 = erased<value>(a);
    REQUIRE(reconcrete_cast<x_t>(t1)->s_ == "hallo");
  }
}
TEST_CASE("lifetime/shared_const_ptr") {
  {
    auto ptr = std::make_shared<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    shared_const_ptr sp1 =
        virtual_void_trait<shared_const_ptr>::construct_from(ptr);
    auto u1 = erased<shared_const_ptr>(ptr);
    A const* a = reconcrete_cast<A>(u1);
    REQUIRE(reconcrete_cast<A>(u1)->s == "hallo");
  }
}
//TEST_CASE("lifetime/unique_ptr") {
//  {
//    auto ptr = std::make_unique<A>("hallo");
//    REQUIRE(ptr->s == "hallo");
//    unique_ptr up1 =
//        virtual_void_trait<unique_ptr>::construct_from(std::move(ptr));
//  }
//  {
//    auto ptr = std::make_unique<A>("hallo");
//    REQUIRE(ptr->s == "hallo");
//    auto u1 = erased<shared_const_ptr>(ptr);
//    A const* a = reconcrete_cast<A>(u1);
//    REQUIRE(reconcrete_cast<A>(u1)->s == "hallo");
//  }
//}
}  // namespace
