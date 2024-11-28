// <!--
#if 0
// -->

<a name="t1"></a>
## virtual_void Tutorial 30: Interface basics

The *virtual_void* lifetime classes are used to transfer typed information with the least possible coupling.  
They are a generalization of *std::any*.

The functional equivalent of `std::any` in *virtual_void* is `virtual_void::data::has_type_info::value`.

A simple usage looks like this:


When you know you have to pass the `value`s of your `vector` to an `ostream`, you can request this behavior via a `virtual_void::interface`.

For our case, we could do it this way:

// <!--
```cpp
#endif begin sample
// -->
#include <iostream>
#include <vector>
#include "catch.hpp"

#include "virtual_void/interface/declare_macro.hpp"
#include "virtual_void/interface/base.hpp"
#include "virtual_void/data/has_no_meta/value.hpp"

namespace tutorial_30_1 {
ERASED_INTERFACE(to_ostream,
                 (INTERFACE_CONST_METHOD(void, draw, std::ostream&)))  // 1
}

namespace tutorial_30_1 {
struct to_ostream_shift_right_v_table_map {  // 3
  void draw(auto* this_, std::ostream& o) { o << *this_; }
};
template <>
struct to_ostream_v_table_map<std::string>  // 2
    : to_ostream_shift_right_v_table_map {};
template <>
struct to_ostream_v_table_map<int> : to_ostream_shift_right_v_table_map {  // 2
};
template <>
struct to_ostream_v_table_map<double>
    : to_ostream_shift_right_v_table_map {  // 2
};

struct A {
  std::string name;
  void draw(std::ostream& o) const { o << name; }  // 4
};
}  // namespace tutorial_30_1

TEST_CASE("tutorial 30/1") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;  // 5
  using namespace tutorial_30_1;
  cout << endl << "*** tutorial 1/4" << endl;

  vector<to_ostream<value>> values{to_ostream<value>("Hello"s),
                                   to_ostream<value>(42),
                                   to_ostream<value>(3.14)};  // 6
  values.emplace_back(A{"world"});                            // 7

  for (auto value : values) value.draw(cout), cout << endl;  // 8
}
// <!-- end of sample
#if 0 
// -->
```

- // 1: Declares the interface: it is named `to_ostream` and has one `const` method. This method `draw` takes one `std::ostream&` parameter.
- // 2: Because `std::string`, `int`, and `double` have no member function `draw`, we must *map* this method for these types. We can do this by specializing the *v_table_map* of `to_ostream`, named `to_ostream_v_table_map`, in the same `namespace` where `to_ostream` was defined.
- // 3: Because `std::string`, `int`, and `double` share the same implementation of `draw`, we delegate it to a helper struct named `to_ostream_shift_right_v_table_map` where we can write `draw` as a function template.
- // 4: `struct A` has a member function `void draw(std::ostream& o) const` which will be chosen by `to_ostream`. This is the default behavior.
- // 5: In this example, we need no *meta info*. So we take a sleeker `value` without any overhead.
- // 6/7: Declares and fills a vector of `values` which support the interface `to_ostream`. In the initializer list, we have to request the *type erasure* by spelling out the target type, whereas in the case of `emplace`, this can be done automatically.
- // 8: The application of interface `to_ostream.draw`.

Curious how things can be further simplified? Take a look at [interface operator overloading](tutorial__33.md).


```cpp
#endif begin sample
// -->

