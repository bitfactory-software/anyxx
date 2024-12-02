// <!--
#if 0
// -->

<a name="t1"></a>
## virtual_void Tutorial 30: Interface basics

The *virtual_void* interfaces combines an *erased liftime handle* with *erased function*(s).

Usually usage is shown with *shape* s . And thats exactly what we are gonna do now:

// <!--
```cpp
#endif begin sample
// -->
#include <iostream>
#include <vector>
#include <catch.hpp>

#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>

namespace tutorial_30_1 {
ERASED_INTERFACE(drawable,
                 (INTERFACE_CONST_METHOD(void, draw, std::ostream&)))  // 1

struct circle { // 2
  double r;
  void draw(std::ostream& o) const { o << "circle with r: " << r; }  
};

struct rectangle { // 3
  double h, w;
  void draw(std::ostream& o) const { o << "rectangle with h: " << h << " x w: " << w; }  
};

}  // namespace tutorial_30_1

TEST_CASE("tutorial 30/1") {
  using namespace std;
  using namespace std::literals::string_literals;
  using namespace virtual_void::data::has_no_meta;  // 4
  using namespace tutorial_30_1;
  cout << endl << "*** tutorial 30/1" << endl;

  vector<drawable<value>> drawables; // 5 
  drawables.emplace_back(circle{1.0}); // 6
  drawables.emplace_back(rectangle{2.0, 3.0});  // 7

  stringstream out;
  for (const auto& drawable : drawables) drawable.draw(out), out << endl;  // 8

  REQUIRE(out.str() == "circle with r: 1\nrectangle with h: 2 x w: 3\n");
}
// <!-- end of sample
#if 0 
```
// -->
- // 1: Declares the `interface`: it is named `drawable` and has one `const` method. This method `draw` takes one `std::ostream&` parameter.
- // 2,3: `circle` and `rectangle` are two simple `struct`s. Both have a `const` method `draw` with a `std::ostream&` parameter.
- // 4: In this example, we need no `meta` info. So we take a sleeker `value` without any overhead.
- // 5: Declares a `std::vector` `drawables` of `value`s which support the `interface` `drawable`.
- // 6/7: add a `circle` and a `rectangle` to drawables.
- // 8: loops over the the `drawables` and calls the `draw` functions of `shape` and `rectangle` via the `interface` `drawable`.

This is the most basic application of an `interface`.

<a name="t2"></a> What hapens, when we want to draw a `std::string`?. The obvious way is to make a

```cpp
struct text {
  std::string s;
  void draw(std::ostream& o) const { ... << s << ... }  
};
```
But a virtual_void::interface offers here a customiazion point, called `v_table_map`.

With this tool, the programm looks like this:

```cpp
#endif begin sample
// -->
#include <iostream>
#include <vector>
#include <string>
#include <catch.hpp>

#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>

namespace tutorial_30_2 {
ERASED_INTERFACE(drawable,
                 (INTERFACE_CONST_METHOD(void, draw, std::ostream&)))

struct circle {
  double r;
  void draw(std::ostream& o) const { o << "circle with r: " << r; }  
};

struct rectangle {
  double h, w;
  void draw(std::ostream& o) const { o << "rectangle with h: " << h << " x w: " << w; }  
};

template <>
struct drawable_v_table_map<std::string> { // 1 
  auto draw(std::string const* s, std::ostream& o) const { // 2
    o << "text via v_table_map: " << *s; // 3
  }
};

}  // namespace tutorial_30_2

TEST_CASE("tutorial 30/2") {
  using namespace std;
  using namespace virtual_void::data::has_no_meta;
  using namespace tutorial_30_2;
  cout << endl << "*** tutorial 30/2" << endl;

  vector<drawable<value>> drawables;
  drawables.emplace_back(circle{1.0});
  drawables.emplace_back(rectangle{2.0, 3.0});
  drawables.emplace_back(std::string{"hello world"}); // 4

  stringstream out;
  for (const auto& drawable : drawables) drawable.draw(out), out << endl;

  REQUIRE(out.str() == "circle with r: 1\nrectangle with h: 2 x w: 3\ntext via v_table_map: hello world\n");
}
// <!-- end of sample
#if 0 
```
// -->

Let us walk through the changes:

- // 1: Specializes the `template` `drawable`*_v_table_map* for `std::string`.
- // 2/3: Implement `draw` for `std::string const*`. Note: You have leeway here!
- // 4: With this in place, we can add a `std::string` to `drawables`

That is it.

```cpp
#endif begin sample
// -->

