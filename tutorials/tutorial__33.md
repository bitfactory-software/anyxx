// <!--
#if 0
// -->

<a name="t1"></a> 
## virtual_void Tutorial 3: Interface Operator Overloading

Let us pick up [this example from tutorial 30](tutorial__30.md).

We are left with an interface that has a function `draw`, which receives a parameter of type `std::ostream`.  
Many types implementing this function with the idiomatic overload of `operator <<`.  
Wouldn't it be nice to use such an overload also for the application of our interface?

Here is how this works out:

```cpp
// <!--
#endif begin sample
// -->
#include <catch.hpp>
#include <iostream>
#include <vector>
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

namespace tutorial_33_1 {
ERASED_INTERFACE(to_ostream,
                 (INTERFACE_CONST_METHOD(void, draw, std::ostream&)))

template <virtual_void::is_virtual_void VV>
std::ostream& operator<<(std::ostream& o, const to_ostream<VV>& i) {  // 1
  i.draw(o);
  return o;
}
}  // namespace tutorial_33_1

namespace tutorial_33_1 {
struct to_ostream_shift_right_v_table_map { // 3
  void draw(auto* this_, std::ostream& o) { o << *this_; }
};
template <>
struct to_ostream_v_table_map<std::string>
    : to_ostream_shift_right_v_table_map {}; // 4a
template <>
struct to_ostream_v_table_map<int> : to_ostream_shift_right_v_table_map {}; // 4b
template <>
struct to_ostream_v_table_map<double> : to_ostream_shift_right_v_table_map {}; // 4c

struct A {
  std::string name;
  void draw(std::ostream& o) const { o << name; }
};
}  // namespace tutorial_33_1

TEST_CASE("tutorial 33/1") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;
  using namespace tutorial_33_1;
  cout << endl << "*** tutorial 33/1" << endl;

  vector<to_ostream<value>> values{to_ostream<value>("Hello"s),
                                   to_ostream<value>(42),
                                   to_ostream<value>(3.14)};
  values.emplace_back(A{"world"});

  stringstream out;
  for (auto value : values) out << value << endl;  // 5

  REQUIRE(out.str() == "Hello\n42\n3.14\nworld\n");
}
// <!-- end of sample
#if 0
// -->
```

- // 1: Here is the override definition of `operator <<` for the interface template `to_ostream<VV>&`. Its implementation calls the interface member function `draw` and returns the `ostream` object as a reference to enable chaining calls.
- // 2: Defines a helper `v_table_map` to delegate `draw` to `operator <<`.
- // 3a/b/c: Specialize the `v_table_map` for `std::string`, `int`, and `double` via the helper `to_ostream_shift_right_v_table_map`.
- // 5: Uses the operator `operator <<` of the interface template `to_ostream<VV>&` as usual to direct the output to `std::cout`.

<a name="t2"></a> 

Because this is a standard pattern, there is a prepared standard solution in [``virtual_void/interface/ostream_shift_left_operator.h``](virtual_void/include\virtual_void/interface\ostream_shift_left_operator.h):
```cpp
#include <ostream>

#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

namespace virtual_void::interface {

ERASED_INTERFACE(ostreamable, 
                 (INTERFACE_CONST_METHOD(void, to_ostream, std::ostream&))) // 1

template <typename T> \\ 2a
concept is_ostreamable = requires(T const& t, std::ostream o) {
  { o << t } -> std::same_as<std::ostream&>;
};

static_assert(is_ostreamable<double>); // 2b

template <typename OSTREAMABLE>
  requires is_ostreamable<OSTREAMABLE>
struct ostreamable_v_table_map<OSTREAMABLE> // 3
    : ostreamable_default_v_table_map<OSTREAMABLE> {
  void to_ostream(OSTREAMABLE const* x, std::ostream& o) { o << (*x); };
};

template <virtual_void::is_virtual_void VV> // 4
std::ostream& operator<<(std::ostream& o, ostreamable<VV> const& i) {  // 4
  i.to_ostream(o);
  return o;
}

};  // namespace virtual_void::interface
```

- // 1 defines an interface `ostreamable` with one member function `to_ostream`, and a parameter `std::ostream&`.
- // 2a introduces the concept `is_ostreamable` to tell us if a type supports the idiomatic usage of `<<` for output streaming.
- // 2b is a quick self-test of `is_ostreamable`.
- // 3 directs the implementation of `ostreamable::to_ostream` for all types conforming to `is_ostreamable` to their corresponding `...<<(std::ostream&...)` operator.
- // 4 defines the `...<<(std::ostream&, const ostreamable<>&)` as pure syntactic sugar.

When applied to our example, this leads to this:

```cpp
// <!--
#endif begin sample
// -->
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/interface/ostream_shift_left_operator.hpp>

#include <iostream>
#include <vector>

#include <catch.hpp>

namespace tutorial_33_2 {

struct A {
  std::string name;
  void to_ostream(std::ostream& o) const { o << name; } // 2a
  // 2b alterantive to 'to_ostream' 
  //friend std::ostream& operator<<(std::ostream& o, const A& a) {
  //  return o << a.name;
  //}
};
}  // namespace tutorial_33_2

TEST_CASE("tutorial 33/2") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::interface;
  using namespace virtual_void::data::has_no_meta;
  using namespace tutorial_33_2;
  cout << endl << "*** tutorial 33/2" << endl;

  vector<ostreamable<value>> values{ostreamable<value>("Hello"s), //1
                                    ostreamable<value>(42),
                                    ostreamable<value>(3.14)};
  values.emplace_back(A{"world"});

  stringstream out;
  for (auto value : values) out << value << endl;  // 5

  REQUIRE(out.str() == "Hello\n42\n3.14\nworld\n");
}
// <!-- end of sample
#if 0
// -->
```

We see:
- // 1 The vector requests `ostreamable<values>`. The standard types all support `std::ostream& operator<<(std::ostream& o, const ...& x)` and are therefore ostreamable.
- // 2a `A` has a member function `to_ostream`, which is picked up because `A` is not `is_ostreamable`.
- // 2b Instead of 2a, we could have given `A` the `<<` operator, which makes `A` also `is_ostreamable`.

- <a name="t3"></a> 
```cpp
// <!--
#endif begin sample
// -->
