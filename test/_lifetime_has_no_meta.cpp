#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "a.hpp"
#include <catch.hpp>


#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/data/has_no_meta/shared_const_ptr.hpp>
#include <virtual_void/data/has_no_meta/unique.hpp>
#include <virtual_void/data/has_no_meta/unique_ptr.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data::has_no_meta;

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
    auto u1 = erased<unique>(1);
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased<unique>(1);
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased_in_place<unique, A>("hallo");
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
}
TEST_CASE("lifetime/shared_const") {
  {
    auto u1 = erased<shared_const>(1);
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    const auto u1 = erased<shared_const>(1);
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased_in_place<shared_const, A>("hallo");
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
}
TEST_CASE("lifetime/value") {
  {
    auto u1 = erased<value>(1);
    REQUIRE(*unchecked_unerase_cast<int>(u1) == 1);
  }
  {
    auto u1 = erased_in_place<value, A>("hallo");
    static_assert(std::same_as<decltype(u1), value>);
    static_assert(
        !std::same_as<std::decay_t<std::remove_pointer_t<decltype(u1)>>, void>);
    auto& u1cr = u1;
    auto a = unchecked_unerase_cast<A>(u1);
    REQUIRE(a->s == "hallo");
    auto a1 = unchecked_unerase_cast<A>(u1cr);
    REQUIRE(a1->s == "hallo");
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
    auto u2 { u1 };
    REQUIRE(unchecked_unerase_cast<A>(u1)->s.data() !=
            unchecked_unerase_cast<A>(u2)->s.data());
    REQUIRE(unchecked_unerase_cast<A>(u2)->s == "hallo");
    unchecked_unerase_cast<A>(u2)->s = "world";
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
    REQUIRE(unchecked_unerase_cast<A>(u2)->s == "world");
  }
  {
    std::string a = "hallo";
    auto t1 = erased<value>(a);
    REQUIRE(*unchecked_unerase_cast<std::string>(t1) == "hallo");
  }
  {
    struct x_t {
      std::string s_;
    };
    x_t a{"hallo"};
    auto t1 = erased<value>(a);
    REQUIRE(unchecked_unerase_cast<x_t>(t1)->s_ == "hallo");
  }
}
TEST_CASE("lifetime/shared_const_ptr") {
  {
    auto ptr = std::make_shared<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    shared_const_ptr sp1 =
        erased_data_trait<shared_const_ptr>::construct_from(ptr);
    auto u1 = erased<shared_const_ptr>(ptr);
    A const* a = unchecked_unerase_cast<A>(u1);
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
}
TEST_CASE("lifetime/unique_ptr") {
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
    A const* a = unchecked_unerase_cast<A>(u1);
    REQUIRE(unchecked_unerase_cast<A>(u1)->s == "hallo");
  }
}
}  // namespace
