// <!--
#if 0
// -->

<a name="t1"></a>
## virtual_void tutoral 1: type erasure and type tunneling basics

The *virtual_void* lifetime classes are used to transfer typed information with least possible coupling.
They are a generalization of *std::any*.

The functional equivalent auf ``std::any`` in *virtual_void* is ``virtual_void::data::has_type_info::value``.

A simple usage looks like this:

// <!--
```cpp
#endif begin sample
// -->
#include <virtual_void/data/has_type_info/value.h>

#include <iostream>
#include <vector>

#include "catch.hpp"

TEST_CASE("tutorial 1/1") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_type_info;
  using namespace virtual_void;
  cout << endl << "*** tutorial 1/1" << endl;

  struct A {
    string name;
  };

  vector<value> values{erased<value>("Hello"s), erased<value>(42),
                       erased<value>(3.14), erased<value>(A{"world"})};

  for (auto value : values)
    if (auto s = unerase_cast<string>(&value)) cout << *s << endl;
    else if(auto i = unerase_cast<int>(&value)) cout << *i << endl;
    else  if(auto d = unerase_cast<double>(&value)) cout << *d << endl;
    else  if(auto a = unerase_cast<A>(&value)) cout << a->name << endl;
    else  cout << "unknown" << endl;
}
// <!-- end of sample
#if 0
// -->
```
If you have used *std::any* before, this should be familar to.
With such a type, you have one word, in our case *value*, with witch you can pass a piece of data around.

The downside, is the long list of ``if``s to unpack the information. 

Also you have to bundle all knowledge of all possible types that are maybe in the ``value`` at the recieving side of the *type tunnel*.

Wouldn't it be nice, to take this burden away from the reciever?.

<a name="t2"></a>

*virtual_void* has here some tools for you. One is an *open method*. 

With such an *open method* in place, our code look like this:


// <!--
```cpp
#endif begin sample
// -->
#include <virtual_void/data/has_type_info/value.h>
#include <virtual_void/open_method/via_type_info/declare.h>

#include <iostream>
#include <vector>

#include "catch.hpp"

TEST_CASE("tutorial 1/2") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_type_info; //1
  using namespace virtual_void::open_method::via_type_info; // 2
  using namespace virtual_void;
  cout << endl << "*** tutorial 1/2" << endl;

  struct A {
    string name;
  };

  vector<value> values{erased<value>("Hello"s), erased<value>(42),
                       erased<value>(3.14), erased<value>(A{"world"})};

  domain domain;
  declare<void(const_, ostream&)> draw{domain}; // 3
  draw.define<string>([](auto* x, ostream& o) { o << *x; }); // 4a
  draw.define<int>([](auto* x, ostream& o) { o << *x; }); // 4b
  draw.define<double>([](auto* x, ostream& o) { o << *x; }); // 4c
  draw.define<A>([](auto* x, ostream& o) { o << x->name; }); // 4d
  draw.seal_for_runtime();

  for (auto value : values) draw(value, cout), cout << endl; // 5
}
// <!-- end of sample
#if 0 
// -->
```

- // 1 We use the erased ``data`` that carries meta information for ``type_info``.
- // 2 We will use an open method ``via_type_info``. This kind of ``open_method`` uses a *perfact hash* from a ``type_info`` to the target function.
- // 3 ``declare`` `s the name and the signature of the open method. The first parameter type must be ``const_`` or ``mutable_``. 
- // 4a-d ``define`` the target function for a type. This function gets registered for the types ``type_info``.
- // 5 because our vaules carry meta information for ``type_info`` (see // 1), we can call to the ``open_method`` ``draw`` with a ``value`` as the first parameter..

This technique allows us to decouple three aspects of one type:
- construction
- usage
- functionality

All three aspects are only coupled via the *vocabulary type* ``value``.

<a name="t3"></a>
If you have a group of types, that support the same compiletime interface for a dessired behaviour, in our case
```cpp
operator <<( std::ostream&, X)
```
the library provides a shortcut for you: 

// <!--
```cpp
#endif begin sample
// -->

#include <virtual_void/data/has_type_info/value.h>
#include <virtual_void/open_method/algorithm.h>
#include <virtual_void/open_method/via_type_info/declare.h>

#include <iostream>
#include <vector>

#include "catch.hpp"

TEST_CASE("tutorial 1/3") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_type_info;
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

  for (auto value : values) draw(value, cout), cout << endl;
}

// <!-- end of sample
#if 0 
// -->
```

``// <-- 1`` shows the call to the algorithm ``fill_with_overloads``. It instanciates the third parameter, (a template function object) for each type argument, and ``declare`` `s that function object for the instanciated type in the open method ``draw``.

The usage of ``open_method::via_type_info`` is on one hand the most easy way to add *type tunneled* functionality, but has in the other a small runtime penalty.

Next we will take a look to th most efficient way to do this with *virtual void* ``interface``s.

<a name="t4"></a>

When you know, you have to pass the ``value``s of your ``vector`` to an ``ostream``, you can request this behavioir via an ``virtual_void::interfcace``.

For our case, we could do it this way:

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

namespace tutorial_1_4 {
ERASED_INTERFACE(to_ostream,
                 (INTERFACE_CONST_METHOD(void, draw, std::ostream&)))  // 1
}

namespace tutorial_1_4 {
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
}  // namespace tutorial_1_4

TEST_CASE("tutorial 1/4") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;  // 5
  using namespace tutorial_1_4;
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

- // 1: Declares the interface: it is named ``to_ostream`` and has one ``const`` method. This method ``draw`` takes one  ``std::ostream&`` parameter.
- // 2: Because ``std::string``,``int`` and ``double`` have no member function ``draw``, we must *map* this method for these types. We can do this be specialicing the *v_table_map* of ``to_ostream`` named ``to_ostream_v_table_map`` in the same ``namespace`` where ``to_ostream`` was definend.
- // 3: Because ``std::string``,``int`` and ``double`` share the same implementation of ``draw`` we delgate it to a helper struct named ``to_ostream_shift_right_v_table_map`` where we can write ``draw`` as a function template.
- // 4: ``struct A`` has a member function ``void draw(std::ostream& o) const`` witch will be choosen by ``to_ostream``. This is the default behaviour.
- // 5: In this example, we need no *meta info*. So we take a sleaker ``value`` without any overhead.
- // 6/7: Declares and fills a vector of ``values`` witch support the interface ``to_ostream``. In the initializer list, we have to request the *type erasure* by spelling out the target type, whereas in the case of ``emplace``, this can be done automatic.
- // 8: The application of interface ``to_ostream.draw``

Curious, how things can be further simplified? Take a look at [interface operator overloading](tutorial___3.md)

```cpp
#endif begin sample
// -->

