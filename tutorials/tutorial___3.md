// <!--
#if 0
// -->

<a name="t1"></a> 
## virtual_void tutoral 3: interface operator overloading

Let us pick up this [this example in tutorial 1](tutorial___1.md/#t4).

We left with an interface, that has a function ``draw``, witch recieves a parameter with type ``ostream``.
All types implementing this function, do this with the ideomatic overload of ``operator <<``.
Wouldn't it be nice, to use such an overload also for the application of our interface?

Here is how this works out:

// <!--
```cpp
#endif begin sample
// -->
#include <virtual_void/data/has_no_meta/value.h>
#include <virtual_void/interface/base.h>
#include <virtual_void/interface/declare_macro.h>

#include <iostream>
#include <vector>

#include "catch.hpp"

namespace tutorial_4_1 {
ERASED_INTERFACE(to_ostream,
                 (INTERFACE_CONST_METHOD(void, draw, std::ostream&)))

template <virtual_void::is_virtual_void VV>
std::ostream& operator<<(std::ostream& o, const to_ostream<VV>& i) { // 1
  i.draw(o);
  return o;
}
}  // namespace tutorial_1_4

namespace tutorial_4_1 {
struct to_ostream_shift_right_v_table_map { 
  void draw(auto* this_, std::ostream& o) { o << *this_; }
};
template <>
struct to_ostream_v_table_map<std::string> 
    : to_ostream_shift_right_v_table_map {};
template <>
struct to_ostream_v_table_map<int> : to_ostream_shift_right_v_table_map {
};
template <>
struct to_ostream_v_table_map<double>
    : to_ostream_shift_right_v_table_map {
};

struct A {
  std::string name;
  void draw(std::ostream& o) const { o << name; }
};
}  // namespace tutorial_1_4

TEST_CASE("tutorial 4/1") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;
  using namespace tutorial_4_1;

  vector<to_ostream<value>> values{to_ostream<value>("Hello"s),
                                   to_ostream<value>(42),
                                   to_ostream<value>(3.14)};
  values.emplace_back(A{"world"});                    

  for (auto value : values) cout << value << endl; // 2
}
// <!-- end of sample
#if 0
// -->
```

We changed two things
- // 1 here is the override definition of ``operator <<`` for the interface template ``to_ostream<VV>&)``. It's implementation calls the interface memberfunction draw and returns the ostream object als reference to enamle chainig calls.
- // 2 uses this operator as usual to direct the output to ``std::cout``

Because this is a standard pattern, there is a prepared standard solution in ``virtual_void``.

<a name="t2"></a> 

// <!--
```cpp
#endif begin sample
// -->


