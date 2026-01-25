#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

struct A {
  A(std::string sp) : s(std::move(sp)) {}
  std::string s;
};

using namespace anyxx;
using namespace anyxx;

namespace {
template <typename CONST_OBSERVER, typename TYPED_MUTABLE_STRING_OBSERVER>
constexpr bool compiles1 =
    requires(CONST_OBSERVER co, TYPED_MUTABLE_STRING_OBSERVER mo) {
      mo = as<std::string>(co);
    };

TEST_CASE("data_erase_unerase/observer") {
  {
    std::string s{"hallo"};
    auto mo = erased<mutable_observer>(s);
    *unchecked_unerase_cast<std::string>(mo) = "world";
    auto co = erased<cref>(s);
    REQUIRE(s == "world");
    REQUIRE(*unchecked_unerase_cast<const std::string>(co) == "world");
    REQUIRE(*unchecked_unerase_cast<std::string>(mo) == "world");
    {
      [[maybe_unused]] const std::string cs;
      // mutable_observer xx(cs);  // shall not compile!
    }
    // tmo = as< std::string >( co ); // shall not compile!
  }
  {
    const std::string s{"hallo"};
    // auto mo = mutable_observer(s);  // shall not compile
    auto co = erased<cref>(s);
    static_assert(std::is_const_v<std::remove_reference_t<std::string const>> ==
                  true);
    REQUIRE(*unchecked_unerase_cast<const std::string>(co) == "hallo");
  }
}
TEST_CASE("data_erase_unerase/unique") {
  {
    auto u1 = erased<unique>(std::make_unique<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1, nullptr) == 1);  // NOLINT
  }
  {
    const auto u1 = erased<unique>(std::make_unique<int>(1));  // NOLINT
    REQUIRE(*unchecked_unerase_cast<int>(u1, nullptr) == 1);
  }
  {
    auto u1 = erased<unique>(std::make_unique<A>(// NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
        "hallo"));  
    REQUIRE(unchecked_unerase_cast<A>(u1, nullptr)->s == "hallo");  // NOLINT
  }
}
TEST_CASE("data_erase_unerase/shared") {
  {
    auto u1 = erased<shared>(std::make_shared<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1, nullptr) == 1);
  }
  {
    const auto u1 = erased<shared>(std::make_shared<int>(1));
    REQUIRE(*unchecked_unerase_cast<int>(u1, nullptr) == 1);
  }
  {
    auto u1 = erased<shared>(std::make_shared<A>("hallo"));
    REQUIRE(unchecked_unerase_cast<A>(u1, nullptr)->s == "hallo");
  }
  {
    auto x = std::make_shared<A const>("hallo");
    auto sc = erased<shared>(x);
    REQUIRE(unchecked_unerase_cast<A>(sc, nullptr)->s == "hallo");
  }
  {
    auto x = std::make_shared<A>("hallo");
    auto sc = erased<shared>(x);
    REQUIRE(unchecked_unerase_cast<A>(sc, nullptr)->s == "hallo");
  }
}
TEST_CASE("data_erase_unerase/shared_const_ptr") {
  {
    auto ptr = std::make_shared<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    shared sp1 = proxy_trait<shared>::erase(ptr);
    auto u1 = erased<shared>(ptr);
    [[maybe_unused]] A const* a = unchecked_unerase_cast<A>(u1, nullptr);
    REQUIRE(unchecked_unerase_cast<A>(u1, nullptr)->s == "hallo");
  }
}
TEST_CASE("data_erase_unerase/unique_ptr") {
  {
    auto ptr = std::make_unique<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    unique up1 = proxy_trait<unique>::erase(std::move(ptr));
    A* a = unchecked_unerase_cast<A>(up1, nullptr);
    REQUIRE(a->s == "hallo");
  }
  {
    auto u1 = erased<unique>(std::make_unique<A>("hallo"));
    A* a = unchecked_unerase_cast<A>(u1, nullptr);
    REQUIRE(a->s == "hallo");
  }
  {
    auto ptr = std::make_unique<A>("hallo");
    REQUIRE(ptr->s == "hallo");
    auto u1 = erased<unique>(std::move(ptr));
    A* a = unchecked_unerase_cast<A>(u1, nullptr);
    REQUIRE(a->s == "hallo");
  }
}
}  // namespace
