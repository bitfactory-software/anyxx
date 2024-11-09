#include "tutorial___2.md"

#include <virtual_void/open_object/members.h>

#include <functional>
#include <iostream>
#include <string>

#include "catch.hpp"

namespace application::core {
struct customer;
using customer_open_object = virtual_void::open_object::members<customer>;  // 1
struct customer : customer_open_object {
  std::string name;
  std::string address;
  std::string vat_number;
};
}  // namespace application::core

namespace application::feature_plugin {
struct usage_hint;  // 2a
using usage_hint_customer_member =
    virtual_void::open_object::member<core::customer, usage_hint,
                                      std::string>;  // 2b
struct usage_hint : usage_hint_customer_member {};   // 2c
}  // namespace application::feature_plugin

const virtual_void::open_object::member_table_index<application::core::customer>
    application::feature_plugin::usage_hint_customer_member::index;  // 3

namespace application::core {
std::function<void(customer&)> hook1;        // 4a
std::function<void(customer const&)> hook2;  // 4b
}  // namespace application::core

namespace application::feature_plugin {
static struct init {
  init() {
    application::core::hook1 = [](core::customer& customer) {  // 5a
      customer[usage_hint{}] = "handle with care";
    };
    application::core::hook2 = [](core::customer const& customer) {  // 5b
      std::cout << *customer.get(usage_hint{}) << std::endl;
    };
  }
} init_here;
}  // namespace application::feature_plugin

TEST_CASE("tutorial 2/1") {
  using namespace application;
  core::customer a_customer{
      .name = "google", .address = "don't mail me", .vat_number = "4711"};
  core::hook1(a_customer);
  core::hook2(a_customer);
}
