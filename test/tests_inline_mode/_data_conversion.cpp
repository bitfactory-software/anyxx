#include <catch.hpp>
#include <string>
#include <anyxx/anyxx.hpp>

using namespace Catch::Matchers;

using namespace anyxx;

TEST_CASE("_data_conversion borrow") {
  std::string s1 = "hallo";
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = borrow_as<mutable_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = borrow_as<mutable_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = borrow_as<mutable_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<shared_const>(std::make_shared<std::string>(s1));
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<unique>(std::make_unique<std::string>(s1));
    auto vv2 = borrow_as<const_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<unique>(std::make_unique<std::string>(s1));
    auto vv2 = borrow_as<mutable_observer>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
  {
    auto vv1 = erased<shared_const>(std::make_shared<std::string>(s1));
    auto vv2 = borrow_as<shared_const>(vv1);
    CHECK(get_void_data_ptr(vv1) == get_void_data_ptr(vv2));
  }
}
TEST_CASE("_data_conversion clone") {
  std::string s1 = "hallo";
  auto& meta_data = get_meta_data<std::string>();
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = clone_to<shared_const>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = clone_to<unique>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = clone_to<shared_const>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = clone_to<unique>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<shared_const>(std::make_shared<std::string>(s1));
    auto vv2 = clone_to<shared_const>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<shared_const>(std::make_shared<std::string>(s1));
    auto vv2 = clone_to<unique>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<unique>(std::make_unique<std::string>(s1));
    auto vv2 = clone_to<shared_const>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
  {
    auto vv1 = erased<unique>(std::make_unique<std::string>(s1));
    auto vv2 = clone_to<unique>(vv1, meta_data);
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(*unchecked_unerase_cast<std::string>(vv1) == s1);
    CHECK(*unchecked_unerase_cast<std::string>(vv2) == s1);
  }
}
TEST_CASE("_data_conversion move") {
  std::string s1 = "hallo";
  {
    auto vv1 = erased<unique>(std::make_unique<std::string>(s1));
    auto vv2 = move_to<unique>(std::move(vv1));
    CHECK(s1 == *unchecked_unerase_cast<std::string>(vv2));
#pragma warning( push )
#pragma warning( disable : 26800)
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(!has_data(vv1));
#pragma warning( pop )
  }
  {
    auto vv1 = erased<unique>(std::make_unique<std::string>(s1));
    auto vv2 = move_to<shared_const>(std::move(vv1));
    CHECK(s1 == *unchecked_unerase_cast<std::string>(vv2));
#pragma warning( push )
#pragma warning( disable : 26800)
    CHECK(get_void_data_ptr(vv1) != get_void_data_ptr(vv2));
    CHECK(!has_data(vv1));
#pragma warning( pop )
  }
}