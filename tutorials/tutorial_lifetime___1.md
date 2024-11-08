// <!--
#if 0
// -->

## virtual_void liftime tutoral: step 1_

The virtual_void lifetime classes are used to transfer typed information with least possible coupling.
They are a generalization of std::any.
The functional equivalent auf std::any in virtual_void is named *virtual_void::data::has_type_info::value**.
A simple usage looks like this:

// <!--
```cpp
//#endif begin sample
// -->
#include <virtual_void/data/has_type_info/value.h>

#include <iostream>
#include <vector>

#include "catch.hpp"

TEST_CASE("tutorial lifetime 1, example 1") {
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
}// <!--
//#if 0 end of sample
// -->
```
// <!--
#endif
// -->
