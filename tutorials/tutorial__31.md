// <!--
#if 0
// -->

<a name="t1"></a>
## virtual_void Tutorial 31/1: Interface Up- and Downcasts

In contrast to some other type erasure libraries, such as [proxy](https://github.com/microsoft/proxy), *virtual_void* interfaces are not optimized to be faster than vanilla C++ virtual functions. They are optimized to retain their type at construction time, so they can be downcasted to it after passing through some other levels of abstraction.

- // 1a/b: In our example, we will use two `namespace`s, `basic_layer` and `shape_layer`, to demonstrate these different abstraction layers.
- // 2: Declares the `interface`: it is named `drawable` and has one `const` method. This method `draw` takes one `std::ostream&` parameter.
- // 3: `draw` is a function. In real-world code, this function would be declared in a .hpp file and defined in a .cpp file.
    - Its first parameter is a `vector` of `drawable`'s `value`s.
    - Its second parameter `pred` is a predicate on `drawable<value>`.
- // 4: The `pred` is used to filter the elements in `drawables` for actual drawing.

So far, the introduction. See below the sample code for further explanations.

//<!--
```cpp
#endif begin sample
// -->

#include <functional>
#include <iostream>
#include <ranges>
#include <vector>

#include <catch.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

namespace tutorial_31_1::basic_layer { //1a
VV_INTERFACE(drawable,
                 (VV_CONST_METHOD(void, draw, std::ostream&))) // 2

using drawable_value = drawable<virtual_void::data::value>;
using drawable_values = std::vector<drawable_value>;
void draw(std::ostream& o, drawable_values& drawables,
          std::function<bool(drawable_value const&)> pred) { // 3
  for (auto drawable : drawables | std::views::filter(pred)) // 4
    drawable.draw(o), std::cout << std::endl;
}
}  // namespace tutorial_31_1::basic_layer

namespace tutorial_31_1::shape_layer { //1b
VV_INTERFACE_(edged, basic_layer::drawable,
                  (VV_CONST_METHOD(int, edges))) // 5

struct circle {
  double r;
  void draw(std::ostream& o) const { o << "circle with r: " << r; }
};
struct rectangle {
  double h, w;
  int edges() const { return 4; } // 6a
  void draw(std::ostream& o) const {
    o << "rectangle with h: " << h << " x w: " << w;
  }
};
struct regular_polygon {
  int sides;
  double side_length;
  int edges() const { return sides; }  // 6b
  void draw(std::ostream& o) const {
    o << "regular_polygon with sides: " << sides
      << ", side_length: " << side_length;
  }
};
}  // namespace tutorial_31_1::shape_layer

TEST_CASE("tutorial 31/1") {
  using namespace std;
  using namespace virtual_void;
  using namespace virtual_void::data::has_no_meta;
  using namespace tutorial_31_1;
  using namespace tutorial_31_1::shape_layer;
  cout << endl << "*** tutorial 31/1" << endl;

  basic_layer::drawable_values drawables; // 7a
  drawables.emplace_back(circle{1.0}); // 7b                           
  drawables.emplace_back(edged<value>{rectangle{2.0, 3.0}}); // 8a     
  drawables.emplace_back(edged<value>{regular_polygon{8, 4.0}}); // 8a

  stringstream out;
  basic_layer::draw(out, drawables, [](basic_layer::drawable_value const& d) {
    if (auto e = interface::v_table_cast<edged<value>>(d)) // 9
      return e->edges() > 4;
    return false;
  });

  REQUIRE(out.str() == "regular_polygon with sides: 8, side_length: 4");
}
// <!-- end of sample
#if 0 
```
-->
- // 5: Declares an `interface` `edged` derived from `drawable`. It adds one more `const` method `edges`.
- // 6a/b: `rectangle` and `regular_polygon` implement this `interface` `edged` via their member function `edges`.
- // 7a/b: `drawables` is a `vector` of `drawable<value>`s and 7b adds a `circle` like in the previous sample [Tutorial 30](tutorial_30).
- // 8a/b: Adds `edged<values>` for a `rectangle` and a `regular_polygon` to `drawables`. This `interface` `edged` is implicitly **upcasted** to `drawable`.
- // 9: We provide for the predicate callback a lambda. Here we test via a `v_table_cast<edged<value>>`, if the drawable is `edged` and eventually returns a **downcasted** `edged`. This can now be used to check if the object should be `draw`n.

This `v_table_cast` has a variant named `unchecked_v_table_cast`. `v_table_cast` corresponds to `dynamic_cast` and `unchecked_v_table_cast` to `static_cast` in vanilla C++.

> ℹ️ 
> For this functionality, it is crucial that the `interface` is constructed with the derivation expected in the callbacks (See 8a in the example above!).

```cpp
#endif begin sample
// -->

