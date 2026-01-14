#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

namespace {
struct X {
  static inline int tracker_ = 0;
  static inline int copy_constructed_ = 0;
  static inline int move_constructed_ = 0;
  static inline int copy_assigned_ = 0;
  static inline int move_assigned_ = 0;
  X(std::string const& s = "") : s_(s) {
    std::println("X({})", s_);
    ++tracker_;
  }
  ~X() {
    std::println("~X({})", s_);
    --tracker_;
  }
  X(X const& x) : s_(x.s_) {
    std::println("X(X const& {})", s_);
    ++tracker_;
    ++copy_constructed_;
  }
  X(X&& x) : s_(std::move(x.s_)) { std::println("X(X&& {})", s_); ++move_constructed_; }
  X& operator=(X const& x) {
    s_ = x.s_;
    std::println("X =(X const& {})", s_);
    ++copy_assigned_;
    return *this;
  }
  X& operator=(X&& x) {
    std::println("X =(X&& {})", s_);
    s_ = std::move(x.s_);
    ++move_assigned_;
    return *this;
  }
  std::string s_;
  [[nodiscard]] std::string operator()() const { return s_; }
};

//ANY(anything, (ANY_OP(std::string, (), (), const)), , )

}  // namespace

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
      REQUIRE((*unerase_cast<X>(u2))() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
}
