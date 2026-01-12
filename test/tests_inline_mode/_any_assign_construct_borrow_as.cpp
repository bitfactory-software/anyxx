#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

using namespace anyxx;

TEST_CASE("assign construct borrow_as") {
  std::string s1 = "hallo";
  {
    auto vv1 = any<const_observer>(s1);
    auto vv2 = borrow_as<any<const_observer>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
  {
    auto vv1 = any<mutable_observer>(s1);
    auto vv2 = borrow_as<any<const_observer>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
  {
    auto vv1 = any<mutable_observer>(s1);
    auto vv2 = borrow_as<any<mutable_observer>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
  {
    auto vv1 = any<shared_const>(std::make_shared<std::string>(s1));
    auto vv2 = borrow_as<any<const_observer>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
  {
    auto vv1 = any<unique>(std::make_unique<std::string>(s1));
    auto vv2 = borrow_as<any<const_observer>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
  {
    auto vv1 = any<unique>(std::make_unique<std::string>(s1));
    auto vv2 = borrow_as<any<mutable_observer>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
  {
    auto vv1 = any<shared_const>(std::make_shared<std::string>(s1));
    auto vv2 = borrow_as<any<shared_const>>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(*vv2));
  }
}
