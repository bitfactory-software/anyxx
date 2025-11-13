#include <catch.hpp>
#include <concepts>
#include <string>
#include <anyxx/anyxx.hpp>

using namespace Catch::Matchers;

using namespace anyxx;
using namespace anyxx;

namespace {
ANY(string_to_string,
             (ANY_OP(std::string, (), (std::string const&), const)))

ANY(string_to_string_mutable,
             (ANY_OP(std::string, (), (), const),
              ANY_OP(std::string, (), (std::string const&))))

}  // namespace test_ns

using namespace anyxx;

namespace {
struct functor_t {
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
struct pure_functor_with_context {
  std::string s_;
  std::string operator()(const std::string& s) const { return s_ + s; }
};
}  // namespace

TEST_CASE("std emulated function") {
  {
    auto functor = std::make_shared<pure_functor_with_context>("hallo");
    string_to_string<shared_const> f{functor};
    REQUIRE(f(" world") == "hallo world");
  }
  {
    string_to_string<value> f{pure_functor_with_context("hallo")};
    REQUIRE(f(" world") == "hallo world");
  }
  {
    string_to_string<shared_const> f{std::make_shared<pure_functor_t>()};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    string_to_string<value> f{pure_functor_t{}};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    string_to_string_mutable<mutable_observer> f{functor};
    REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(f() == "hallo world");
    REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ ==
            "hallo world");

    string_to_string_mutable<const_observer> fc{f};
    REQUIRE(fc() == "hallo world");

    functor_t fx{"hallo world"};
    string_to_string_mutable<const_observer> fc1{fx};
    REQUIRE(fc1() == "hallo world");

    // does not work, because functor lives only during construction of
    // string_to_string_mutable
    // string_to_string_mutable<const_observer> fc2{functor_t{"hallo
    // world"}}; CHECK(fc2() == "hallo world"); // access of member is invalid
  }
  {
    string_to_string_mutable<value> fc2{functor_t{"hallo world"}}; 
    CHECK(fc2() == "hallo world");
  }
  {
    pure_functor_t pf{};
    string_to_string<mutable_observer> f{pf};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    string_to_string<const_observer> f{
        pure_functor_t{}};  // thos works, because 'pure'
    REQUIRE(f("hello world") == "hello world");
  }
  {
    string_to_string_mutable<unique> f{std::make_unique<functor_t>("hello")};
    REQUIRE(f(" world") == "hello");
    REQUIRE(unchecked_unerase_cast<functor_t>(f)->s_ ==
            "hello world");
    static_assert(!std::assignable_from<string_to_string_mutable<unique>,
                                        string_to_string_mutable<unique>>);
    string_to_string_mutable<unique> f2{std::move(f)};
    REQUIRE(!has_data(get_erased_data(f)));
    REQUIRE(f2(", bye") == "hello world");
    REQUIRE(unchecked_unerase_cast<functor_t>(f2)->s_ ==
            "hello world, bye");
  }

  {
    string_to_string<const_observer> sts{[](std::string const& in) {
      return in + " world!";
    }};  // works, because pure
    CHECK(sts("hello") == "hello world!");
  }
}
