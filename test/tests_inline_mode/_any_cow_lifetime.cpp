#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

namespace anyxx {}  // namespace anyxx

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

TRAIT_(getset, anyxx::dynamic_copyable,
       (ANY_FN(std::string const&, get, (), const),
        ANY_FN(void, set, (std::string const&), )))

using any_getset = any<getset, cow>;

}  // namespace

TEST_CASE("cow lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset u{std::in_place_type<X>, "hallo"};
      CHECK(u.get() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset u{std::in_place_type<X>, "hallo"};
      CHECK(u.get() == "hallo");
      CHECK(get_proxy(u).unique());
      CHECK(X::tracker_ == 1);
      any_getset u2{std::in_place_type<X>, "world"};
      CHECK(u2.get() == "world");
      CHECK(get_proxy(u2).unique());
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_proxy(u).holder_ == nullptr);  // NOLINT
      CHECK(u2.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_proxy(u2).holder_ == nullptr);  // NOLINT
      CHECK(u3.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any_getset>(std::move(u3));
      CHECK(get_proxy(u3).holder_ == nullptr);  // NOLINT
      CHECK(u4.get() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset v{std::in_place_type<X>, "hallo"};
      CHECK(v.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = v;
      CHECK(X::tracker_ == 1);
      CHECK(get_proxy(v2).holder_ == get_proxy(v).holder_);
      CHECK(v.get() == "hallo");
      CHECK(v2.get() == "hallo");
      v2 = X{"world!"};
      CHECK(get_proxy(v).holder_->count_ == 1);
      CHECK(X::tracker_ == 2);
      CHECK(get_proxy(v2).holder_ != get_proxy(v).holder_);
      CHECK(v.get() == "hallo");
      CHECK(v2.get() == "world!");
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset v{std::in_place_type<X>, "hallo"};
      CHECK(v.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = clone_to<any_getset>(v);
      CHECK(X::tracker_ == 1);
      CHECK(get_proxy(*v2).holder_ == get_proxy(v).holder_);
      CHECK(v.get() == "hallo");
      CHECK(v2->get() == "hallo");
      CHECK(get_proxy(*v2).holder_ == get_proxy(v).holder_);
      CHECK(get_proxy(v).holder_->count_ == 2);
      static_assert(is_copy_constructor_v_table<any_getset::v_table_t>);
      v2->set("hallo world");
      CHECK(v.get() == "hallo");
      CHECK(v2->get() == "hallo world");
      CHECK(get_proxy(*v2).holder_ != get_proxy(v).holder_);
      CHECK(get_proxy(*v2).unique());
      CHECK(get_proxy(v).unique());
    }
    CHECK(X::tracker_ == 0);
    X::tracker_ = 0;
  }
}
