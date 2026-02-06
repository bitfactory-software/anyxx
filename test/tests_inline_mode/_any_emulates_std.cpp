#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_function.hpp>
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <print>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

namespace {
ANY(string_to_string, (ANY_OP(std::string, (), (std::string const&), const)), )
ANY(string_to_string_mutable,
    (ANY_OP(std::string, (), (), const),
     ANY_OP(std::string, (), (std::string const&), )), )

}  // namespace

using namespace anyxx;

namespace {
struct functor_t {
  static inline int tracker_ = 0;
  functor_t(std::string s = {}) : s_(std::move(s)) {
    ++tracker_;
    std::println("{}", tracker_);
  }
  functor_t(functor_t const&) = default;
  functor_t(functor_t&&) = default;
  functor_t& operator=(functor_t const&) = default;
  functor_t& operator=(functor_t&&) = default;
  ~functor_t() {  // NOLINT
    --tracker_;
    std::println("{}", tracker_);
  }

  std::string s_;
  std::string operator()() const { return s_; }
  std::string operator()(const std::string& s) {
    auto ret = s_;
    s_ += s;
    return ret;
  }
};
struct pure_functor_t {
  std::string operator()() const { return "literal"; }
  std::string operator()(const std::string& s) const { return s; }
};
static_assert(std::is_trivial_v<pure_functor_t>);

struct pure_functor_with_context {
  std::string s_;
  std::string operator()(const std::string& s) const { return s_ + s; }
};
}  // namespace

TEST_CASE("std emulated function") {
  {
    auto functor = std::make_shared<pure_functor_with_context>("hallo");
    any_string_to_string<shared> f{functor};
    REQUIRE(f(" world") == "hallo world");
  }
  {
    any_string_to_string<val> f{pure_functor_with_context("hallo")};
    REQUIRE(f(" world") == "hallo world");
  }
  {
    any_string_to_string<shared> f{std::make_shared<pure_functor_t>()};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    any_string_to_string<val> f{pure_functor_t{}};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    any_string_to_string_mutable<mutref> f{functor};
    REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(f() == "hallo world");
    REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ == "hallo world");

    any_string_to_string_mutable<cref> fc{f};
    REQUIRE(fc() == "hallo world");

    functor_t fx{"hallo world"};
    any_string_to_string_mutable<cref> fc1{fx};
    REQUIRE(fc1() == "hallo world");

    // does not work, because functor lives only during construction of
    // string_to_string_mutable
    // string_to_string_mutable<cref> fc2{functor_t{"hallo
    // world"}}; CHECK(fc2() == "hallo world"); // access of member is invalid
  }
  {
    any_string_to_string_mutable<val> fc2{functor_t{"hallo world"}};
    CHECK(fc2() == "hallo world");
  }
  {
    pure_functor_t pf{};
    ;
    any_string_to_string<mutref> f{pf};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    pure_functor_t pf{};
    any_string_to_string<cref> f{pf};  // works, because 'pure'
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t::tracker_ = 0;
    {
      {
        any_string_to_string_mutable<unique> f{std::make_unique<functor_t>()};
        REQUIRE(functor_t::tracker_ == 1);
        REQUIRE(f("hello") == "");
        REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ == "hello");
      }
      REQUIRE(functor_t::tracker_ == 0);
    }
    any_string_to_string_mutable<unique> f{
        std::make_unique<functor_t>("hello")};
    REQUIRE(f(" world") == "hello");
    REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ == "hello world");
    static_assert(!std::assignable_from<any_string_to_string_mutable<unique>,
                                        any_string_to_string_mutable<unique>>);
    any_string_to_string_mutable<unique> f2{std::move(f)};
    REQUIRE(!get_proxy_ptr(f));  // NOLINT
    REQUIRE(f2(", bye") == "hello world");
    REQUIRE(unchecked_unerase_cast<functor_t>(f2)->s_ == "hello world, bye");
  }

  {
    auto f = [](std::string const& in) { return in + " world!"; };
    any_string_to_string<cref> sts{f};  // works, because pure
    auto hello_world = sts("hello");
    static_assert(std::is_same_v<decltype(hello_world), std::string>);
    CHECK(hello_world == "hello world!");

    any<cref, function<std::string(std::string const&), const_>> any_f_cref{f};
    CHECK(any_f_cref("C++") == "C++ world!");
    any<cref, function<std::string(std::string const&), mutable_>> any_f_mref{
        f};

    any<using_<decltype(f)&>, function<std::string(std::string const&), const_>>
        any_f_by_val{f};
    CHECK(any_f_by_val("static C++") == "static C++ world!");

    CHECK(trait_as<function<std::string(std::string const&), const_>>(f)(
              "trait_as") == "trait_as world!");
  }
}
