#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>

#include "a.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;

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
    *unchecked_unerase_cast<std::string>(mo) = "world";
    auto co = erased<const_observer>(s);
    REQUIRE(s == "world");
    REQUIRE(*unchecked_unerase_cast<const std::string>(co) == "world");
    REQUIRE(*unchecked_unerase_cast<std::string>(mo) == "world");
    {
      const std::string cs;
      // mutable_observer xx(cs);  // shall not compile!
    }
    // tmo = as< std::string >( co ); // shall not compile!
  }
  {
    const std::string s{"hallo"};
    // auto mo = mutable_observer(s);  // shall not compile
    auto co = erased<const_observer>(s);
    static_assert(std::is_const_v<std::remove_reference_t<std::string const>> ==
                  true);
    REQUIRE(*unchecked_unerase_cast<const std::string>(co) == "hallo");
  }
}
TEST_CASE("lifetime/unique") {
  {
    auto u1 = erased<unique>(std::make_unique<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased<unique>(std::make_unique<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased<unique>(std::make_unique<A>("hallo"));
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
}
TEST_CASE("lifetime/shared_const") {
  {
    auto u1 = erased<shared_const>(std::make_shared<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased<shared_const>(std::make_shared<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased<shared_const>(std::make_shared<A>("hallo"));
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
  {
    auto x = std::make_shared<A const>("hallo");
    auto sc = erased<shared_const>(x);
    REQUIRE(unchecked_unerase_cast<A>(sc)->s == "hallo");
  }
  {
    auto x = std::make_shared<A>("hallo");
    auto sc = erased<shared_const>(x);
    REQUIRE(unchecked_unerase_cast<A>(sc)->s == "hallo");
  }
}
TEST_CASE("lifetime/shared_const_ptr") {
  {
    auto ptr = std::make_shared<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    shared_const sp1 = trait<shared_const>::construct_from(ptr);
    auto u1 = erased<shared_const>(ptr);
    A const* a = unchecked_unerase_cast<A>(u1);
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
}
TEST_CASE("lifetime/unique_ptr") {
  {
    auto ptr = std::make_unique<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    unique up1 = trait<unique>::construct_from(std::move(ptr));
    A* a = unchecked_unerase_cast<A>(up1);
    REQUIRE(a->s == "hallo");
  }
  {
    auto u1 = erased<unique>(std::make_unique<A>("hallo"));
    A* a = unchecked_unerase_cast<A>(u1);
    REQUIRE(a->s == "hallo");
  }
  {
    auto ptr = std::make_unique<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    auto u1 = erased<unique>(std::move(ptr));
    A* a = unchecked_unerase_cast<A>(u1);
    REQUIRE(a->s == "hallo");
  }
}
}  // namespace
