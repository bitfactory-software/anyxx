#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

namespace {

template <typename T=std::string>
struct XX {
  static inline int tracker_ = 0;
  static inline int copy_constructed_ = 0;
  static inline int move_constructed_ = 0;
  static inline int copy_assigned_ = 0;
  static inline int move_assigned_ = 0;
  XX(T const& s = "") : s_(s) {
    std::println("X({})", s_);
    ++tracker_;
    std::println("X::tracker_ {})", tracker_);
  }
  ~XX() {
    std::println("~X({} moved = {})", s_, moved_);
    if (!moved_) --tracker_;
    std::println("X::tracker_ {})", tracker_);
  }
  XX(XX const& x) : s_(x.s_) {
    std::println("X(X const& {})", s_);
    ++tracker_;
    ++copy_constructed_;
    std::println("X::copy_constructed_ {})", copy_constructed_);
    std::println("X::tracker_ {})", tracker_);
  }
  XX(XX&& x) : s_(std::move(x.s_)) {
    std::println("X(X&& {})", s_);
    x.moved_ = true;
    ++move_constructed_;
    std::println("X::move_constructed_ {})", move_constructed_);
    std::println("X::tracker_ {})", tracker_);
  }
  XX operator=(XX const& x) {
    s_ = x.s_;
    std::println("X =(X const& {})", s_);
    ++copy_assigned_;
    std::println("X::tracker_ {})", tracker_);
    return *this;
  }
  XX& operator=(XX&& x) {
    std::println("X =(X&& {})", s_);
    x.moved_ = true;
    s_ = std::move(x.s_);
    ++move_assigned_;
    std::println("X::tracker_ {})", tracker_);
    return *this;
  }
  T s_;
  bool moved_ = false;
  [[nodiscard]] T operator()() const { return s_; }
};

using X = XX<>;


}  // namespace

TEST_CASE("value lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> u{std::in_place_type<X>, "hallo"};
      REQUIRE((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> u{std::in_place_type<X>, "hallo"};
      REQUIRE((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
      any<value> u2{std::in_place_type<X>, "world"};
      REQUIRE((*unerase_cast<X>(u2))() == "world");
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      REQUIRE(get_void_data_ptr(u) == nullptr);
      REQUIRE((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      REQUIRE(get_void_data_ptr(u2) == nullptr);
      REQUIRE((*unerase_cast<X>(u3))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any<value>>(std::move(u3));
      REQUIRE(get_void_data_ptr(u3) == nullptr);
      REQUIRE((*unerase_cast<X>(u4))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> v{std::in_place_type<X>, "hallo"};
      REQUIRE((*unerase_cast<X>(v))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = v;
      CHECK(X::tracker_ == 2);
      REQUIRE(get_void_data_ptr(v2) != get_void_data_ptr(v));
      REQUIRE((*unerase_cast<X>(v))() == "hallo");
      REQUIRE((*unerase_cast<X>(v2))() == "hallo");
      v2 = X{"world!"};
      CHECK(X::tracker_ == 2);
      REQUIRE(get_void_data_ptr(v2) != get_void_data_ptr(v));
      REQUIRE((*unerase_cast<X>(v))() == "hallo");
      REQUIRE((*unerase_cast<X>(v2))() == "world!");
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> v{std::in_place_type<X>, "hallo"};
      REQUIRE((*unerase_cast<X>(v))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = clone_to<any<value>>(v);
      CHECK(X::tracker_ == 2);
      REQUIRE(get_void_data_ptr(*v2) != get_void_data_ptr(v));
      REQUIRE((*unerase_cast<X>(v))() == "hallo");
      REQUIRE((*unerase_cast<X>(*v2))() == "hallo");
    }
    CHECK(X::tracker_ == 0);
    X::tracker_ = 0;
  }
}

TEST_CASE("unique lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any<unique> u{std::make_unique<X>("hallo")};
      REQUIRE((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<unique> u{std::make_unique<X>("hallo")};
      REQUIRE((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
      any<unique> u2{std::make_unique<X>("world")};
      REQUIRE((*unerase_cast<X>(u2))() == "world");
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      REQUIRE(get_void_data_ptr(u) == nullptr);
      REQUIRE((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      REQUIRE(get_void_data_ptr(u2) == nullptr);
      REQUIRE((*unerase_cast<X>(u3))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any<unique>>(std::move(u3));
      REQUIRE(get_void_data_ptr(u3) == nullptr);
      REQUIRE((*unerase_cast<X>(u4))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
}

TEST_CASE("v-table lifetime") {
    X::move_constructed_ = 0;
    {
    CHECK(X::tracker_ == 0);
    {
      basic_any_v_table v_table_x(std::in_place_type<X>);
      auto ptr = v_table_x.allocate();
      CHECK(X::tracker_ == 0);
      X* x_ptr = nullptr;
      {
        X x{"hallo"};
        CHECK(X::tracker_ == 1);
        x_ptr = static_cast<X*>(copy_construct_at(&v_table_x, ptr, &x));
        CHECK(X::tracker_ == 2);
      }
      CHECK(x_ptr);
      CHECK(x_ptr == ptr);
      CHECK(X::tracker_ == 1);
      CHECK((*x_ptr)() == "hallo");
      v_table_x.delete_(ptr);
      CHECK(X::tracker_ == 0);
    }
  }
  CHECK(X::tracker_ == 0);
  {
      CHECK(X::move_constructed_ == 0);
      basic_any_v_table v_table_x(std::in_place_type<X>);
    auto ptr = v_table_x.allocate();
    CHECK(X::tracker_ == 0);
    X* x_ptr = nullptr;
    {
      X x{"hallo"};
      CHECK(X::move_constructed_ == 0);
      CHECK(X::tracker_ == 1);
      x_ptr = static_cast<X*>(move_construct(&v_table_x, ptr, &x));
      CHECK(X::move_constructed_ == 1);
      CHECK(X::tracker_ == 1);
      CHECK(x.moved_);
    }
    CHECK(x_ptr);
    CHECK(x_ptr == ptr);
    CHECK(X::tracker_ == 1);
    CHECK((*x_ptr)() == "hallo");
    v_table_x.delete_(ptr);
    CHECK(X::tracker_ == 0);
    CHECK(X::move_constructed_ == 1);
  }
}
