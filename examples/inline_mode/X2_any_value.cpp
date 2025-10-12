#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <virtual_void/any_meta_function.hpp>
#include <virtual_void/value.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

namespace {

VV_ANY(any)

}  // namespace

TEST_CASE("example X2/ any value") {
  any<value> a1{std::string{"hello world"}};
  CHECK(*unerase_cast<std::string>(a1) == "hello world");
  CHECK(get_runtime(a1).get_type_info() == typeid(std::string));
}
