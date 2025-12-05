#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>

struct forward_declared_first;
struct declared_first{};
TEST_CASE("is_type_complete1") {
  static_assert(!anyxx::is_type_complete<forward_declared_first>);
  CHECK(!anyxx::is_type_complete<forward_declared_first>);
  static_assert(anyxx::is_type_complete<declared_first>);
  CHECK(anyxx::is_type_complete<declared_first>);
}
struct declared_first;
struct forward_defined_first {};
TEST_CASE("is_type_complete2") {
  static_assert(!anyxx::is_type_complete<forward_declared_first>); // state of first time seen!
  CHECK(!anyxx::is_type_complete<forward_declared_first>);
  static_assert(anyxx::is_type_complete<declared_first>);
  CHECK(anyxx::is_type_complete<declared_first>);
}