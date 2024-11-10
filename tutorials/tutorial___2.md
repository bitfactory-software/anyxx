// <!--
#if 0
// -->

<a name="t1"></a> 
## virtual_void tutoral 2: Open objects

In interpreted langugages like *python* and *java script*, one can easily enhance an object at runtime. 
This is a usefull feature.
For example, you have some objects of type customer in the core of your system.
Here you keep al the usual name, address and payment data.
One day, one of your plugins, need *only* one more field. (/irony off)
This is with an language like above no problem. 
In contrary to c++.

When you foresee such demands and have library support for *open objects*, you can adapt the classes, where such a demand could arise.
(Or at least, you can adapt the class the first time you encounter this scenario.)

Let us step through an example:

// <!--
```cpp
#endif begin sample
// -->
#include <virtual_void/open_object/members.h>

#include <functional>
#include <iostream>
#include <string>

#include "catch.hpp"

namespace application::core {
struct customer; // 1a
using customer_open_object = virtual_void::open_object::members<customer>;  // 1b
struct customer : customer_open_object { // 1c
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
  std::cout << std::endl << "*** tutorial 2/1" << std::endl;
  core::customer a_customer{
      .name = "google", .address = "don't mail me", .vat_number = "4711"};
  core::hook1(a_customer);
  core::hook2(a_customer);
}
// <!-- end of sample
#if 0
// -->
```

- // 1a-c prepare ``customer`` for usage with ``open_object::members``.
- // 2a-c define the interface for the ``open_object::member`` ``usage_count`` in ``customer`` with type ``std::string``.
- // 3 defines the static storeage, where the index of ``usage_count`` in the ``members`` ``std::vector`` is stored.
- // 4a/b are inversion of control ``hook``s to call into plugin code.
- // 5a/b fills the hooks with the plugin logic. Here we access ``usage_count`` in ``customer``.

// <!--
```cpp
#endif begin sample
// -->


