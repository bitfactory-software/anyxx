#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace anyxx;

namespace {

// ANY(any, , , )

}  // namespace

TEST_CASE("example X2/ any value") {
  any<value> a1{std::string{"hello world"}};
  static_assert(anyxx::is_any<decltype(a1)>);
  CHECK(*unerase_cast<std::string>(a1) == "hello world");
  CHECK(get_meta_data(a1).get_type_info() == typeid(std::string));
  auto a2 = a1;  // copy
  CHECK(*unerase_cast<std::string>(a2) == "hello world");
  CHECK(get_meta_data(a2).get_type_info() == typeid(std::string));
  a2 = 42;
  CHECK(*unerase_cast<std::string>(a1) == "hello world");
  CHECK(get_meta_data(a1).get_type_info() == typeid(std::string));
  CHECK(*unerase_cast<int>(a2) == 42);
  CHECK(get_meta_data(a2).get_type_info() == typeid(int));
}

