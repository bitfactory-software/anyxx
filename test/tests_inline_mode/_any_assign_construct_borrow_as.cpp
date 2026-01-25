#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

TEST_CASE("assign construct borrow_as") {
  std::string s1 = "hallo";
  {
    using from_t = any<cref>;
    using to_t = any<cref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t(s1);
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;  // NOLINT
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};// NOLINT
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<mutref>;
    using to_t = any<cref>;
    auto vv1 = from_t(s1);
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<mutref>;
    using to_t = any<mutref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t(s1);
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;// NOLINT
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};// NOLINT
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<shared>;
    using to_t = any<cref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t(std::make_shared<std::string>(s1));
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<unique>;
    using to_t = any<cref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t{std::make_unique<std::string>(s1)};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<unique>;
    using to_t = any<mutref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t{std::make_unique<std::string>(s1)};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<shared>;
    using to_t = any<shared>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto sp = std::make_shared<std::string>(s1);
    auto vv1 = from_t{sp};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;// NOLINT
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};// NOLINT
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<weak>;
    using to_t = any<weak>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto sp = std::make_shared<std::string>(s1);
    std::weak_ptr<std::string> wp = sp;
    auto vv1 = from_t{wp};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;// NOLINT
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};// NOLINT
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<shared>;
    using to_t = any<weak>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t{std::make_shared<std::string>(s1)};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*lock(*vv2)));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(*lock(vv3)) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(*lock(vv4)) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<val>;
    using to_t = any<cref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t{s1};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
  {
    using from_t = any<val>;
    using to_t = any<mutref>;
    static_assert(borrowable_from<to_t::proxy_t, from_t::proxy_t>);
    auto vv1 = from_t{s1};
    auto vv2 = borrow_as<to_t>(vv1);
    CHECK(get_proxy_ptr(vv1) == get_proxy_ptr(*vv2));
    to_t vv3 = vv1;
    CHECK(get_proxy_ptr(vv3) == get_proxy_ptr(vv1));
    to_t vv4{vv1};
    CHECK(get_proxy_ptr(vv4) == get_proxy_ptr(vv1));
  }
}
