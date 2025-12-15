#include <catch2/catch_test_macros.hpp>
#include <string>
#include <bit_factory/anyxx.hpp>

using namespace anyxx;

namespace {

ANY(any,)

}  // namespace

TEST_CASE("example X2/ any value") {
  any<value> a1{std::string{"hello world"}};
  CHECK(*unerase_cast<std::string>(a1) == "hello world");
  CHECK(get_meta_data(a1).get_type_info() == typeid(std::string));
}
