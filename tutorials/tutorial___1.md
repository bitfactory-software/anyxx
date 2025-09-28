// <!--
#if 0
// -->

<a name="t1"></a>
## virtual_void Tutorial 1: Type Erasure and Type Tunneling Basics

The *virtual_void* lifetime classes are used to transfer typed information with the least possible coupling.  
They are a generalization of *std::any*.

The functional equivalent of `std::any` in *virtual_void* is `virtual_void::data::value`.

A simple usage looks like this:

// <!--
```cpp
#endif begin sample
// -->
#include <catch.hpp>
#include <iostream>
#include <vector>
#include <virtual_void/data/value.hpp>

TEST_CASE("tutorial 1/1") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;
  using namespace virtual_void;
  cout << endl << "*** tutorial 1/1" << endl;

  struct A {
    string name;
  };

  vector<value> values{erased<value>("Hello"s), erased<value>(42),
                       erased<value>(3.14), erased<value>(A{"world"})};
  stringstream out;
  for (auto value : values)
    if (auto s = unerase_cast<string>(&value))
      out << *s << endl;
    else if (auto i = unerase_cast<int>(&value))
      out << *i << endl;
    else if (auto d = unerase_cast<double>(&value))
      out << *d << endl;
    else if (auto a = unerase_cast<A>(&value))
      out << a->name << endl;
    else
      out << "unknown" << endl;

  REQUIRE(out.str() == "Hello\n42\n3.14\nworld\n");
}
// <!-- end of sample
#if 0
// -->
```
If you have used *std::any* before, this should be familiar to you.  
With such a type, you have one word, in our case *value*, with which you can pass a piece of data around.

The downside is the long list of `if` statements to unpack the information.  

Additionally, you have to bundle all knowledge of all possible types that might be in the `value` on the receiving side of the *type tunnel*.

Wouldn't it be nice to take this burden away from the receiver?

<a name="t2"></a>

*virtual_void* provides some tools for this. One of them is an *open method*.  

With such an *open method* in place, our code looks like this:


```cpp
// <!--
#endif begin sample
// -->
#include <iostream>
#include <vector>

#include <catch.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/open_method/via_type_info/declare.hpp>

TEST_CASE("tutorial 1/2") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;         // 1
  using namespace virtual_void::open_method::via_type_info;  // 2
  using namespace virtual_void;
  cout << endl << "*** tutorial 1/2" << endl;

  struct A {
    string name;
  };

  vector<value> values{erased<value>("Hello"s), erased<value>(42),
                       erased<value>(3.14), erased<value>(A{"world"})};
  domain domain;
  declare<void(const_, ostream&)> draw{domain};               // 3
  draw.define<string>([](auto* x, ostream& o) { o << *x; });  // 4a
  draw.define<int>([](auto* x, ostream& o) { o << *x; });     // 4b
  draw.define<double>([](auto* x, ostream& o) { o << *x; });  // 4c
  draw.define<A>([](auto* x, ostream& o) { o << x->name; });  // 4d
  draw.seal_for_runtime();

  stringstream out;
  for (auto value : values) draw(value, out), out << endl;  // 5

  REQUIRE(out.str() == "Hello\n42\n3.14\nworld\n");
}
// <!-- end of sample
#if 0 
// -->
```

- // 1 We use the erased `data` that carries meta-information for `type_info`.
- // 2 We will use an open method `via_type_info`. This kind of `open_method` uses a *perfect hash* from a `type_info` to the target function.
- // 3 `declare` declares the name and the signature of the `open_method`. The first parameter type must be `const_` or `mutable_`.
- // 4a-d `define` defines the target function for a type. This function gets registered for the type's `type_info`.
- // 5 Because our values carry meta-information for `type_info` (see // 1), we can call the `open_method` `draw` with a `value` as the first parameter.

This technique allows us to decouple three aspects of one type:
- construction
- usage
- functionality

All three aspects are only coupled via the *vocabulary type* `value`.

<a name="t3"></a>
If you have a group of types that support the same compile-time interface for a desired behavior, in our case:

```cpp
operator <<( std::ostream&, X)
```
the library provides a shortcut for you: 

// <!--
```cpp
#endif begin sample
// -->
#include <iostream>
#include <vector>
#include <catch.hpp>

#include <virtual_void/data/value.hpp>
#include <virtual_void/open_method/via_type_info/declare.hpp>
#include <virtual_void/open_method/algorithm.hpp>

TEST_CASE("tutorial 1/3") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;
  using namespace virtual_void::open_method::via_type_info;
  using namespace virtual_void;
  cout << endl << "*** tutorial 1/3" << endl;

  struct A {
    string name;
  };

  vector<value> values{erased<value>("Hello"s), erased<value>(42),
                       erased<value>(3.14), erased<value>(A{"world"})};

  domain domain;
  declare<void(const_, ostream&)> draw{domain};
  open_method::fill_with_overloads<string, int, double>(
      draw, [](auto* x, ostream& o) { o << *x; }); // 1
  draw.define<A>([](auto* x, ostream& o) { o << x->name; });
  draw.seal_for_runtime();

  stringstream out;
  for (auto value : values) draw(value, out), out << endl;

  REQUIRE(out.str() == "Hello\n42\n3.14\nworld\n");
}
// <!-- end of sample
#if 0 
// -->
```

- // 1 shows the call to the algorithm `fill_with_overloads`. It instantiates the second parameter (a template function object) for each type argument, and `define`s that function object for the instantiated type in the open method `draw`.

The usage of `open_method::via_type_info` is, on one hand, the easiest way to add *type tunneled* functionality, but, on the other hand, it comes with a small get_meta_data penalty.

Next, we will take a look at the most efficient way to do this with *virtual_void*: `Interface`s.

// <!--
```cpp
#endif begin sample
// -->
