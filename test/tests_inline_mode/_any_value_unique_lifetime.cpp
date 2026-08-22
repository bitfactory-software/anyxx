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
    interior_ptr_ = &s_;
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
    interior_ptr_ = &s_;
    std::println("X(X const& {})", s_);
    ++tracker_;
    ++copy_constructed_;
    std::println("X::copy_constructed_ {})", copy_constructed_);
    std::println("X::tracker_ {})", tracker_);
  }
  XX(XX&& x) : s_(std::move(x.s_)) {  // NOLINT
    interior_ptr_ = &s_;
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
  T const& get() const { return s_; }
  void set(T const& value) { s_ = value; }
  T* interior_ptr_ = nullptr;
  T s_;
  bool moved_ = false;
  [[nodiscard]] T operator()() const {
    CHECK(interior_ptr_ == &s_);
    return s_;
  }
};

using X = XX<>;
static_assert(sizeof(X) > anyxx::small_object_size);
using Y = XX<int>;
static_assert(sizeof(Y) <= anyxx::small_object_size);
static_assert(std::is_trivial_v<int>);
static_assert(!std::is_trivial_v<Y>);
static_assert(!std::is_trivially_move_constructible_v<Y>);
static_assert(!std::is_trivially_move_assignable_v<Y>);
static_assert(!std::is_trivially_copy_constructible_v<Y>);
static_assert(!std::is_trivially_copy_assignable_v<Y>);
static_assert(sizeof(int) <= anyxx::small_object_size);

TRAIT_(getset, anyxx::dynamic_copyable,
       (ANY_FN(std::string const&, get, (), const),
        ANY_FN(void, set, (std::string const&), )))

}  // namespace

TEST_CASE("val<> lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> u{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> u{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
      any<dynamic_copyable> u2{std::in_place_type<X>, "world"};
      CHECK((*unerase_cast<X>(u2))() == "world");
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_proxy_ptr_const(u) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_proxy_ptr_const(u2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u3))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any<dynamic_copyable>>(std::move(u3));
      CHECK(get_proxy_ptr_const(u3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u4))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = v;
      CHECK(X::tracker_ == 1);
      CHECK(get_proxy_ptr_const(v2) == get_proxy_ptr_const(v));
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK((*unerase_cast<X>(v2))() == "hallo");
      v2 = X{"world!"};
      CHECK(X::tracker_ == 2);
      CHECK(get_proxy_ptr_const(v2) != get_proxy_ptr_const(v));
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK((*unerase_cast<X>(v2))() == "world!");
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v{std::in_place_type<X>, "hallo"};
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = clone_to<any<dynamic_copyable>>(v);
      CHECK(X::tracker_ == 1);
      CHECK(get_proxy_ptr_const(*v2) == get_proxy_ptr_const(v));
      CHECK((*unerase_cast<X>(v))() == "hallo");
      CHECK((*unerase_cast<X>(*v2))() == "hallo");
    }
    CHECK(X::tracker_ == 0);
    X::tracker_ = 0;
  }
}

TEST_CASE("val<> lifetime small object") {
  {
    CHECK(Y::tracker_ == 0);
    {
      any<dynamic_copyable> u{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(u))() == 42);
      CHECK(Y::tracker_ == 1);
    }
    CHECK(Y::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    {
      any<dynamic_copyable> u{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(u))() == 42);
      CHECK(Y::tracker_ == 1);
      any<dynamic_copyable> u2{std::in_place_type<Y>, 100};
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
      auto u4 = move_to<any<dynamic_copyable>>(std::move(u3));
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
      any<dynamic_copyable> v{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK(Y::tracker_ == 1);
      auto v2 = v;
      CHECK(Y::tracker_ == 2);
      CHECK(get_proxy_ptr_const(v2) != get_proxy_ptr_const(v));
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK((*unerase_cast<Y>(v2))() == 42);
      v2 = Y{100};
      CHECK(Y::tracker_ == 2);
      CHECK(get_proxy_ptr_const(v2) != get_proxy_ptr_const(v));
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK((*unerase_cast<Y>(v2))() == 100);
    }
    CHECK(Y::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    {
      any<dynamic_copyable> v{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK(Y::tracker_ == 1);
      auto v2 = clone_to<any<dynamic_copyable>>(v);
      CHECK(Y::tracker_ == 2);
      CHECK(get_proxy_ptr_const(*v2) != get_proxy_ptr_const(v));
      CHECK((*unerase_cast<Y>(v))() == 42);
      CHECK((*unerase_cast<Y>(*v2))() == 42);
    }
    CHECK(Y::tracker_ == 0);
    Y::tracker_ = 0;
  }
}

TEST_CASE("value lifetime trivial object") {
  {
    {
      any<dynamic_copyable> u{std::in_place_type<int>, 42};
      CHECK((*unerase_cast<int>(u)) == 42);
    }
  }
  {
    {
      any<dynamic_copyable> u{std::in_place_type<int>, 42};
      CHECK((*unerase_cast<int>(u)) == 42);
      any<dynamic_copyable> u2{std::in_place_type<int>, 100};
      CHECK((*unerase_cast<int>(u2)) == 100);
      u2 = std::move(u);
      CHECK(get_v_table(u) == nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(u2)) == 42);
      auto u3 = std::move(u2);
      CHECK(get_v_table(u2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(u3)) == 42);
      auto u4 = move_to<any<dynamic_copyable>>(std::move(u3));
      CHECK(get_v_table(u3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(u4)) == 42);
    }
  }
  {
    {
      any<dynamic_copyable> v{std::in_place_type<int>, 42};
      CHECK((*unerase_cast<int>(v)) == 42);
      auto v2 = v;
      CHECK(get_proxy_ptr_const(v2) != get_proxy_ptr_const(v));
      CHECK((*unerase_cast<int>(v)) == 42);
      CHECK((*unerase_cast<int>(v2)) == 42);
      v2 = int{100};
      CHECK(get_proxy_ptr_const(v2) != get_proxy_ptr_const(v));
      CHECK((*unerase_cast<int>(v)) == 42);
      CHECK((*unerase_cast<int>(v2)) == 100);
    }
  }
  {
    {
      any<dynamic_copyable> v{std::in_place_type<int>, 42};
      CHECK((*unerase_cast<int>(v)) == 42);
      auto v2 = clone_to<any<dynamic_copyable>>(v);
      CHECK((*unerase_cast<int>(v)) == 42);
      CHECK((*unerase_cast<int>(*v2)) == 42);
    }
  }
}

TEST_CASE("unique lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      auto c = std::make_unique<X>("hallo");
      any<dynamic_deletable, unique> u{std::move(c)};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_deletable, unique> u{std::make_unique<X>("hallo")};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_deletable, unique> u{std::make_unique<X>("hallo")};
      CHECK((*unerase_cast<X>(u))() == "hallo");
      CHECK(X::tracker_ == 1);
      any<dynamic_deletable, unique> u2{std::make_unique<X>("world")};
      CHECK((*unerase_cast<X>(u2))() == "world");
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_proxy_ptr_const(u) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_proxy_ptr_const(u2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(u3))() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any<dynamic_deletable, unique>>(std::move(u3));
      CHECK(get_proxy_ptr_const(u3) == nullptr);  // NOLINT
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
      dynamic_copyable_v_table v_table_x(std::in_place_type<X>);
      auto ptr = allocate(&v_table_x);
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
      v_table_x.destructor(ptr);
      delete ptr;
      CHECK(X::tracker_ == 0);
    }
  }
  CHECK(X::tracker_ == 0);
  {
    CHECK(X::move_constructed_ == 0);
    dynamic_copyable_v_table v_table_x(std::in_place_type<X>);
    auto ptr = allocate(&v_table_x);
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
    v_table_x.destructor(ptr);
    delete ptr;
    CHECK(X::tracker_ == 0);
    CHECK(X::move_constructed_ == 1);
  }
}

TEST_CASE("v-table lifetime small object") {
  Y::move_constructed_ = 0;
  {
    CHECK(Y::tracker_ == 0);
    {
      dynamic_copyable_v_table v_table_x(std::in_place_type<Y>);
      auto ptr = allocate(&v_table_x);
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
      v_table_x.destructor(ptr);
      delete ptr;
      CHECK(Y::tracker_ == 0);
    }
  }
  CHECK(Y::tracker_ == 0);
  {
    CHECK(Y::move_constructed_ == 0);
    dynamic_copyable_v_table v_table_x(std::in_place_type<Y>);
    auto ptr = allocate(&v_table_x);
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
    v_table_x.destructor(ptr);
    delete ptr;
    CHECK(Y::tracker_ == 0);
    CHECK(Y::move_constructed_ == 1);
  }
}

TEST_CASE("val<> lifetime trivial/small/big object") {
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 0);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<dynamic_copyable> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v1 = v2;
      CHECK(get_v_table(v1) != nullptr);  // NOLINT
      CHECK(get_v_table(v2) != nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(v1))() == "hello");
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 1);
      any<dynamic_copyable> v3{std::in_place_type<int>, 42};
      v1 = v3;
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 1);
      CHECK(get_v_table(v1) != nullptr);  // NOLINT
      CHECK(get_v_table(v3) != nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(v1)) == 42);
      CHECK((*unerase_cast<int>(v3)) == 42);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<dynamic_copyable> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v2 = v1;
      CHECK(get_v_table(v1) != nullptr);  // NOLINT
      CHECK(get_v_table(v2) != nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(v2))() == 42);
      CHECK(Y::tracker_ == 2);
      CHECK(X::tracker_ == 0);
      any<dynamic_copyable> v3{std::in_place_type<int>, 41};
      v2 = v3;
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 0);
      CHECK(get_v_table(v2) != nullptr);  // NOLINT
      CHECK(get_v_table(v3) != nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(v2)) == 41);
      CHECK((*unerase_cast<int>(v3)) == 41);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<dynamic_copyable> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v1 = std::move(v2);
      CHECK(get_v_table(v2) == nullptr);  // NOLINT
      CHECK((*unerase_cast<X>(v1))() == "hello");
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 1);
      any<dynamic_copyable> v3{std::in_place_type<int>, 42};
      v1 = std::move(v3);
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 0);
      CHECK(get_v_table(v1) != nullptr);  // NOLINT
      CHECK(get_v_table(v3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(v1)) == 42);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    {
      any<dynamic_copyable> v1{std::in_place_type<Y>, 42};
      CHECK((*unerase_cast<Y>(v1))() == 42);
      CHECK(Y::tracker_ == 1);
      any<dynamic_copyable> v2{std::in_place_type<X>, "hello"};
      CHECK((*unerase_cast<X>(v2))() == "hello");
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 1);
      v2 = std::move(v1);
      CHECK(get_v_table(v1) == nullptr);  // NOLINT
      CHECK((*unerase_cast<Y>(v2))() == 42);
      CHECK(Y::tracker_ == 1);
      CHECK(X::tracker_ == 0);
      any<dynamic_copyable> v3{std::in_place_type<int>, 42};
      v2 = std::move(v3);
      CHECK(Y::tracker_ == 0);
      CHECK(X::tracker_ == 0);
      CHECK(get_v_table(v2) != nullptr);  // NOLINT
      CHECK(get_v_table(v3) == nullptr);  // NOLINT
      CHECK((*unerase_cast<int>(v2)) == 42);
    }
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
  }
  CHECK(Y::tracker_ == 0);
  CHECK(X::tracker_ == 0);
  {
    any<dynamic_copyable> v1{std::in_place_type<int>, 41};
    CHECK((*unerase_cast<int>(v1)) == 41);
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    any<dynamic_copyable> v2{std::in_place_type<X>, "hello"};
    CHECK((*unerase_cast<X>(v2))() == "hello");
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 1);
    v1 = std::move(v2);
    CHECK(get_v_table(v2) == nullptr);  // NOLINT
    CHECK((*unerase_cast<X>(v1))() == "hello");
    CHECK(get_v_table(v1) != nullptr);  // NOLINT
    CHECK(get_v_table(v2) == nullptr);  // NOLINT
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 1);
  }
  CHECK(Y::tracker_ == 0);
  CHECK(X::tracker_ == 0);
  {
    any<dynamic_copyable> v1{std::in_place_type<int>, 41};
    CHECK((*unerase_cast<int>(v1)) == 41);
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    any<dynamic_copyable> v2{std::in_place_type<Y>, 42};
    CHECK((*unerase_cast<Y>(v2))() == 42);
    CHECK(Y::tracker_ == 1);
    CHECK(X::tracker_ == 0);
    v1 = std::move(v2);
    CHECK(get_v_table(v2) == nullptr);  // NOLINT
    CHECK((*unerase_cast<Y>(v1))() == 42);
    CHECK(get_v_table(v1) != nullptr);  // NOLINT
    CHECK(get_v_table(v2) == nullptr);  // NOLINT
    CHECK(Y::tracker_ == 1);
    CHECK(X::tracker_ == 0);
  }
  CHECK(Y::tracker_ == 0);
  CHECK(X::tracker_ == 0);
  {
    any<dynamic_copyable> v1{std::in_place_type<int>, 41};
    CHECK((*unerase_cast<int>(v1)) == 41);
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    any<dynamic_copyable> v2{std::in_place_type<X>, "hello"};
    CHECK((*unerase_cast<X>(v2))() == "hello");
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 1);
    v1 = v2;
    CHECK((*unerase_cast<X>(v1))() == "hello");
    CHECK((*unerase_cast<X>(v2))() == "hello");
    CHECK(get_v_table(v1) != nullptr);
    CHECK(get_v_table(v2) != nullptr);
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 1);
  }
  CHECK(Y::tracker_ == 0);
  CHECK(X::tracker_ == 0);
  {
    any<dynamic_copyable> v1{std::in_place_type<int>, 41};
    CHECK((*unerase_cast<int>(v1)) == 41);
    CHECK(Y::tracker_ == 0);
    CHECK(X::tracker_ == 0);
    any<dynamic_copyable> v2{std::in_place_type<Y>, 42};
    CHECK((*unerase_cast<Y>(v2))() == 42);
    CHECK(Y::tracker_ == 1);
    CHECK(X::tracker_ == 0);
    v1 = v2;
    CHECK((*unerase_cast<Y>(v1))() == 42);
    CHECK((*unerase_cast<Y>(v2))() == 42);
    CHECK(get_v_table(v1) != nullptr);
    CHECK(get_v_table(v2) != nullptr);
    CHECK(Y::tracker_ == 2);
    CHECK(X::tracker_ == 0);
  }
  CHECK(Y::tracker_ == 0);
  CHECK(X::tracker_ == 0);
}

TEST_CASE("val<> lifetime get/set cow") {
  any<getset> v2{std::in_place_type<X>, "hello"};
  CHECK(v2.get() == "hello");
  CHECK(X::tracker_ == 1);
  auto v1 = v2;
  CHECK(X::tracker_ == 1);
  CHECK(v1.get() == "hello");
  CHECK(v2.get() == "hello");
  v1.set("world");
  CHECK(X::tracker_ == 2);
  CHECK(v1.get() == "world");
  CHECK(v2.get() == "hello");
  CHECK(get_v_table(v1) != nullptr);
  CHECK(get_v_table(v2) != nullptr);
}