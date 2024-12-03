// <!--
#if 0
// -->

<a name="t1"></a> 
## virtual_void Tutorial 2: Open Objects

In interpreted languages like *Python* and *JavaScript*, one can easily enhance an object at runtime.  
This is a useful feature.  
For example, you have some objects of type `Customer` in the core of your system.  
Here you keep all the usual name, address, and payment data.  
One day, one of your plugins needs *only* one more field. (/irony off)  
This is no problem with one of the above languages.  

In contrast to C++.

When you foresee such demands and have the right library support for *open objects*, you can prepare the classes where such a demand could arise.  
(Or at least, you can adapt the class the first time you encounter this scenario.)

Let us step through an example:

// <!--
```cpp
#endif begin sample
// -->
#include <functional>
#include <iostream>
#include <string>
#include <virtual_void/open_object/members.hpp>

#include <catch.hpp>

namespace application::core {
struct customer : virtual_void::open_object::members<customer> {  // 1
  std::string name;
  std::string address;
  std::string vat_number;
};
}  // namespace application::core

namespace application::feature_plugin {
inline const virtual_void::open_object::member<core::customer, std::string>
    usage_hint;  // 2
}  // namespace application::feature_plugin

namespace application::core {
std::function<void(customer&)> hook1;                         // 3a
std::function<void(std::ostream& o, customer const&)> hook2;  // 3b
}  // namespace application::core

namespace application::feature_plugin {
static struct init {
  init() {
    application::core::hook1 = [](core::customer& customer) {  // 4a
      customer[usage_hint] = "handle with care";
    };
    application::core::hook2 = [](std::ostream& o,
                                  core::customer const& customer) {  // 4b
      o << *customer.get(usage_hint) << std::endl;
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
  std::stringstream out;
  core::hook2(out, a_customer);

  REQUIRE(out.str() == "handle with care\n");
}
// <!-- end of sample
#if 0
// -->
```

- // 1 prepare ``customer`` for usage with ``open_object::members``.
- // 2 define the ``open_object::member`` ``usage_hint`` in ``customer`` with type ``std::string``.
- // 3a/b are inversion of control ``hook``s to call into plugin code.
- // 4a/b fills the hooks with the plugin logic. Here we access ``usage_hint`` in ``customer``.

// <!--
```cpp
#endif begin sample
// -->
