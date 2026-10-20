#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/anys/function.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

// base architecture layer
TRAIT(base, dynamic_castable, 
      (FN_IMPL(public, std::string, as_string, (), const,
               () { return std::format("{}", self); })))

std::string process_with(std::vector<any<base>> const& things,
                         any<function, std::string(any<base> const&)> process) {
  std::string result;
  for (auto const& thing : things) {
    result += process(thing) + "\n";
  }
  return result;
}

// higher architecture layer
TRAIT(derived, base,
      (FN_IMPL(public, std::string, decorated, (), const,
               () { return +"exact: " + any<base>{self}.as_string(); })))

std::string decoration_handler(any<base> const& thing) {
  if (auto d = anyxx::downcast_to<any<derived>>(thing)) {
    return d->decorated();
  } else {
    return "possible answer: " + thing.as_string();
  }
};

}  // namespace

TEST_CASE("talk_23_7_downcast") {
  int i = 4711;
  std::string s = "hello world";

  auto processed =
      process_with({any<derived>{i}, any<derived>{s}, 42}, decoration_handler);
  CHECK(processed ==
        "exact: 4711\nexact: hello world\npossible answer: 42\n");
}
