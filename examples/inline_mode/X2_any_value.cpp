#include <catch.hpp>
#include <string>
#include <anyxx/anyxx.hpp>

using namespace Catch::Matchers;

using namespace anyxx;

namespace {

ANY(any)

}  // namespace

TEST_CASE("example X2/ any value") {
  any<value> a1{std::string{"hello world"}};
  CHECK(*unerase_cast<std::string>(a1) == "hello world");
  CHECK(get_meta_data(a1).get_type_info() == typeid(std::string));
}
