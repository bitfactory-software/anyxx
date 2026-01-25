#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

TEST_CASE("_data_conversion clone") {
  std::string s1 = "hallo";
  {
    auto vv1 = any<const_observer>(s1);
    static_assert(anyxx::is_any<decltype(vv1)>);
    auto vv2 = clone_to<any<shared>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<const_observer>(s1);
    auto vv2 = clone_to<any<unique>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<mutable_observer>(s1);
    auto vv2 = clone_to<any<shared>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<mutable_observer>(s1);
    auto vv2 = clone_to<any<unique>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<shared>(std::make_shared<std::string>(s1));
    auto vv2 = clone_to<any<shared>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<shared>(std::make_shared<std::string>(s1));
    auto vv2 = clone_to<any<unique>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<unique>(std::make_unique<std::string>(s1));
    auto vv2 = clone_to<any<shared>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
  {
    auto vv1 = any<unique>(std::make_unique<std::string>(s1));
    auto vv2 = clone_to<any<unique>>(vv1);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(*vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(*vv2) == s1);
  }
}
// cppcheck-suppress-begin [accessMoved]
TEST_CASE("_data_conversion move") {
  std::string s1 = "hallo";
  {
    auto vv1 = any<unique>(std::make_unique<std::string>(s1));
    auto vv2 = move_to<any<unique>>(std::move(vv1));
    CHECK(s1 == *unchecked_unerase_cast<std::string>(vv2));
    static_assert(anyxx::is_any<decltype(vv1)>);
    static_assert(anyxx::is_any<decltype(vv2)>);
#pragma warning(push)
#pragma warning(disable : 26800)
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));  // NOLINT
    CHECK(!has_data(get_erased_data(vv1), nullptr));
#pragma warning(pop)
  }
  {
    auto vv1 = any<unique>(std::make_unique<std::string>(s1));
    auto vv2 = move_to<any<shared>>(std::move(vv1));
    CHECK(s1 == *unchecked_unerase_cast<std::string>(vv2));
#pragma warning(push)
#pragma warning(disable : 26800)
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));  // NOLINT
    CHECK(!has_data(get_erased_data(vv1), nullptr));
#pragma warning(pop)
  }
}
// cppcheck-suppress-end [accessMoved]
