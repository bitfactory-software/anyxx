// <!--
#if 0
// -->

## virtual_void tutoral: step 1_

The *virtual_void* lifetime classes are used to transfer typed information with least possible coupling.
They are a generalization of *std::any*.
The functional equivalent auf *std::any* in *virtual_void* is *virtual_void::data::has_type_info::value**.
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
With such a type, you have one word, in our case *value*, with witch you can the pice of data around.
The downside, is the long list of "if's" to unpack the information. 
Also you have to bundle all knowledge of all possible types that are maybe in the *value* at the recieving side of the *type tunnel*.
Wouldn't it be nice, to take this burden away from the reciever?.

*virtual_void* has here some tools for you. One is an *open method*. That would look like this:
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
  using namespace virtual_void::data::has_type_info;
  using namespace virtual_void::open_method::via_type_info;
  using namespace virtual_void;

  struct A {
    string name;
  };

  vector<value> values{erased<value>("Hello"s), erased<value>(42),
                       erased<value>(3.14), erased<value>(A{"world"})};

  domain domain;
  declare<void(void* const, ostream&)> draw{domain};
  draw.define<string>([](auto* x, ostream& o) { o << *x; });
  draw.define<int>([](auto* x, ostream& o) { o << *x; });
  draw.define<double>([](auto* x, ostream& o) { o << *x; });
  draw.define<A>([](auto* x, ostream& o) { o << x->name; });
  draw.seal_for_runtime();

  for (auto value : values)
      draw(value, cout), cout << endl;
}
// <!-- end of sample
#if 0 
// -->

// <!--
#endif
// -->
