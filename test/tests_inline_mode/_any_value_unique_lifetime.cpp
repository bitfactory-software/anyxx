#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

namespace {

template <typename T = std::string>
struct XX {
  static inline int tracker_ = 0;
  static inline int copy_constructed_ = 0;
  static inline int move_constructed_ = 0;
  static inline int copy_assigned_ = 0;
  static inline int move_assigned_ = 0;
  XX(T s = "") : s_(std::move(s)) {
    std::println("X({})", s_);
    ++tracker_;
    std::println("X::tracker_ {})", tracker_);
  }
  ~XX() {  // NOLINT
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
  XX(XX&& x) : s_(std::move(x.s_)) {  // NOLINT
    std::println("X(X&& {})", s_);
    x.moved_ = true;
    ++move_constructed_;
    std::println("X::move_constructed_ {})", move_constructed_);
    std::println("X::tracker_ {})", tracker_);
  }
  XX& operator=(XX const& x) {
    s_ = x.s_;
    std::println("X =(X const& {})", s_);
    ++copy_assigned_;
    std::println("X::tracker_ {})", tracker_);
    return *this;
  }
  XX& operator=(XX&& x) {  // NOLINT
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
static_assert(sizeof(X) >= sizeof(mutable_void));
using Y = XX<int>;
static_assert(sizeof(Y) == sizeof(mutable_void));

}  // namespace

TEST_CASE("value lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> u{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> u{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
      any<value> u2{std::in_place_type<X>, "world"};
      CHECK((*unerase_cast<X>(u2))() == "world");
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_void_data_ptr(u) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_void_data_ptr(u2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u3))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any<value>>(std::move(u3));
      CHECK(get_void_data_ptr(u3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u4))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> v{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = v;
      CHECK(X::tracker_ == 2);
      CHECK(get_void_data_ptr(v2) != get_void_data_ptr(v));
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK((*unerase_cast<X>(v2))() == "hallo");
      v2 = X{"world!"};
      CHECK(X::tracker_ == 2);
      CHECK(get_void_data_ptr(v2) != get_void_data_ptr(v));
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK((*unerase_cast<X>(v2))() == "world!");
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<value> v{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = clone_to<any<value>>(v);
      CHECK(X::tracker_ == 2);
      CHECK(get_void_data_ptr(*v2) != get_void_data_ptr(v));
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK((*unerase_cast<X>(*v2))() == "hallo");
    }
    CHECK(X::tracker_ == 0);
    X::tracker_ = 0;
  }
}

TEST_CASE("value lifetime small object") {
  {
    CHECK(Y::tracker_ == 0);
    {
      any<value> u{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(u))() == 42);
      CHECK(Y::tracker_ == 1);
    }
    CHECK(Y::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    {
      any<value> u{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(u))() == 42);
      CHECK(Y::tracker_ == 1);
      any<value> u2{std::in_place_type<Y>, 100};
      CHECK((*unerase_cast<Y>(u2))() == 100);
      CHECK(Y::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_v_table(u) == nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(u2))() == 42);
      CHECK(Y::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_v_table(u2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(u3))() == 42);
      CHECK(Y::tracker_ == 1);
      auto u4 = move_to<any<value>>(std::move(u3));
      CHECK(get_v_table(u3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(u4))() == 42);
      CHECK(Y::tracker_ == 1);
    }
    CHECK(Y::tracker_ == 0);
    Y::tracker_ = 0;
  }
  {
    CHECK(Y::tracker_ == 0);
    {
      any<value> v{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK(Y::tracker_ == 1);
      auto v2 = v;
      CHECK(Y::tracker_ == 2);
      CHECK(get_void_data_ptr(v2) != get_void_data_ptr(v));
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK((*unerase_cast<Y>(v2))() == 42);
      v2 = Y{100};
      CHECK(Y::tracker_ == 2);
      CHECK(get_void_data_ptr(v2) != get_void_data_ptr(v));
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK((*unerase_cast<Y>(v2))() == 100);
    }
    CHECK(Y::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    {
      any<value> v{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK(Y::tracker_ == 1);
      auto v2 = clone_to<any<value>>(v);
      CHECK(Y::tracker_ == 2);
      CHECK(get_void_data_ptr(*v2) != get_void_data_ptr(v));
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK((*unerase_cast<Y>(*v2))() == 42);
    }
    CHECK(Y::tracker_ == 0);
    X::tracker_ = 0;
  }
}

TEST_CASE("unique lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any<unique> u{std::make_unique<X>("hallo")};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<unique> u{std::make_unique<X>("hallo")};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
      any<unique> u2{std::make_unique<X>("world")};
      CHECK((*unerase_cast<X>(u2))() == "world");
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_void_data_ptr(u) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_void_data_ptr(u2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u3))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any<unique>>(std::move(u3));
      CHECK(get_void_data_ptr(u3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u4))() == "hallo");
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
      x_ptr = static_cast<X*>(move_construct_at(&v_table_x, ptr, &x));
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

TEST_CASE("v-table lifetime small object") {
  Y::move_constructed_ = 0;
  {
    CHECK(Y::tracker_ == 0);
    {
      basic_any_v_table v_table_x(std::in_place_type<Y>);
      auto ptr = v_table_x.allocate();
      CHECK(Y::tracker_ == 0);
      Y* x_ptr = nullptr;
      {
        Y x{42};
        CHECK(Y::tracker_ == 1);
        x_ptr = static_cast<Y*>(copy_construct_at(&v_table_x, ptr, &x));
        CHECK(Y::tracker_ == 2);
      }
      CHECK(x_ptr);
      CHECK(x_ptr == ptr);
      CHECK(Y::tracker_ == 1);
      CHECK((*x_ptr)() == 42);
      v_table_x.delete_(ptr);
      CHECK(Y::tracker_ == 0);
    }
  }
  CHECK(Y::tracker_ == 0);
  {
    CHECK(Y::move_constructed_ == 0);
    basic_any_v_table v_table_x(std::in_place_type<Y>);
    auto ptr = v_table_x.allocate();
    CHECK(Y::tracker_ == 0);
    Y* x_ptr = nullptr;
    {
      Y x{42};
      CHECK(Y::move_constructed_ == 0);
      CHECK(Y::tracker_ == 1);
      x_ptr = static_cast<Y*>(move_construct_at(&v_table_x, ptr, &x));
      CHECK(Y::move_constructed_ == 1);
      CHECK(Y::tracker_ == 1);
      CHECK(x.moved_);
    }
    CHECK(x_ptr);
    CHECK(x_ptr == ptr);
    CHECK(Y::tracker_ == 1);
    CHECK((*x_ptr)() == 42);
    v_table_x.delete_(ptr);
    CHECK(Y::tracker_ == 0);
    CHECK(Y::move_constructed_ == 1);
  }
}

TEST_CASE("value lifetime small/big object") {
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<value> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<value> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v1 = v2;
      CHECK(get_v_table(v1) != nullptr);  // NOLINT
      CHECK(get_v_table(v2) != nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(v1))() == "hello");
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 2);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<value> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<value> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v2 = v1;
      CHECK(get_v_table(v1) != nullptr);  // NOLINT
      CHECK(get_v_table(v2) != nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(v2))() == 42);
      CHECK(Y::tracker_ == 2);
      CHECK(X::tracker_ == 0);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<value> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<value> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v1 = std::move(v2);
      CHECK(get_v_table(v2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(v1))() == "hello");
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 1);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<value> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<value> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v2 = std::move(v1);
      CHECK(get_v_table(v1) == nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(v2))() == 42);
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 0);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
}
