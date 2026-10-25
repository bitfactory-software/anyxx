#include <algorithm>
#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <format>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

struct data {};

TRAIT(source, dynamic_castable, )

TRAIT(greetable, dynamic_castable, 
  (FN(std::string, greet, (), const))
) 

TRAIT_MAP(data, greetable){
  std::string greet(const data&) { return "Hello"; }
};

enable_dynamic<data, greetable> __;

}  // namespace

TEST_CASE("talk_23_8_crosscast") {
  using namespace anyxx;

  auto d = data{};
  auto s = any<source>{d};
  auto g = *borrow_as<any<greetable>>(s);
  CHECK(g.greet() == "Hello");
}

