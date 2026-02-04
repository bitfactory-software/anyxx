// <!--
// The MIT License (MIT)
//
// Copyright (c) 2026 Bit Factory Software GmbH
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// MAGIC for this README.cpp
// https://stackoverflow.com/questions/49980639/specify-alternate-project-level-readme-md-on-github
// mklink in PowerShell
// cmd /c mklink README.md ..\examples\inline_mode\README.cpp
//
#if 0
// -->
[Hello World!](#showcase1) / [Model Map](#showcase2) / [Type Erased Spaceship](#showcase3) / [Open Dispatch As Visiitor](#showcase4) 
/ [Crosscast + Factory = Serialization](#showcase5) / [Basic *Any++* std::variant usage](#showcase6) 
/ [Basic *Any++* open std::variant usage: 'vany'](#showcase7)


[![MIT Licence](http://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/license/mit)
[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)
[![Static Badge](https://img.shields.io/badge/dos-Any%2B%2B-blue)](https://www.alexweb.io/anyxx/)

# Any++ : How to ``trait`` ``any`` ``virtual``, ``static`` or ``variant``?

<img width="908" height="760" alt="Any++logo-small" src="https://github.com/user-attachments/assets/e4766dd4-7a21-486d-9dd0-8725149e0754" />


**Any++** enhances the well-known *boost::any* and *std::any* with:

- **Flexible choice of underlying storage mechanisms**
  - Type-erased
  - Concrete
  - Variant
- **Extensibility**
  - Ability to add functions and operators using Rust-like **traits** (including templates)
  - Support for **open (multi-)dispatch**
  - Runtime support for **downcasts and cross-casts**

To allow easy **separation of concerns**, the library adds utilities for:
- Factories
- Hooks
- Load-time extendable members

The performance of **dynamic dispatch is on par with virtual functions**. 

The combination of static and dynamic dispatch enables 
- a hybrid usage of `std::variant` and **type erasure** to balance performance and coupling.
- C++0x-like concept maps for static and dynamic customization points and default behavior.

<a name="showcase1"></a> 
### Showcase 1: Basic *Any++* usage
```cpp
// <!--
#endif
// -->
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace showcase1 {
struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

TRAIT(drawable, (ANY_FN(std::string, draw, (), const)))

void draw(std::stringstream& os,
          std::vector<anyxx::any<anyxx::val, drawable>> const& drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

TEST_CASE("Showcase1") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Hello\nWorld\n");
}
}  // namespace showcase1
// <!--
#if 0
// -->
```
Showcase 1 demonstrates the most basic usage of the Any++ library for type-erased 
polymorphism in C++. It defines two simple types, circle and square, each with a draw() method 
returning a string. Using the Any++ macro system, a drawable trait is declared, 
specifying the required interface (draw() const -> std::string).

The draw function takes a vector of type-erased any objects that conform to the drawable trait.
It shows how to create and use a heterogeneous collection of unrelated objects (circle and square).

[Compiler Explorer] **TODO**

[More Showcases here...](#showcase2) [Docs](https://www.alexweb.io/anyxx/)

### Available via vcpkg

### Useage in CMakeLists.txt:
```
FetchContent_Declare(
    bit_factory::anyxx
    GIT_REPOSITORY C
    GIT_TAG main
)
FetchContent_MakeAvailable(anyxx)
```

### Clone the repo:
```
git clone -c core.symlinks=true git clone -c core.symlinks=true <repo-url>
```

### Performace compared
| Benchmark     | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>MS Visual C++ 18.0.1 /O2 /Ob2 | AMD Ryzen 9<br> 5900X 12-Core Processor (3.70 GHz)<br>MS Visual C++ 17.14.14 /O2 /Ob2 | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>clang-cl /O2 /Ob2 |
|:----------------------------|--------------:|-------:|-------:|
| **Single dispatch** |   | | |
| virtual function (reference*) |  100% | 100% | 100% |
| any++ interface |  **100%** | **100%** | **100%** |
| any++ open method | **115%** | **200%** | **130%** | 
| **Double dispatch** |   | |  |
| std::variant + std::visit (reference*) | 100% | 100% | 100% |
| hand rolled w. virtual function  | 150% | 150%| 400% |
| any++ open method | **120%** | **150%** | **300%**(*) |

- reference*:
  - 100% in different colums do not compare<br>
  - 100% **Single dispatch** does not compare to 100% **Double dispatch**
- any++ open method vs std::variant + std::visit with clang:
  - the meassured time with std::varaint/std::visit multidispatch on clang is 40%(!) from the MS Visual C++ result.

### CI Matrix
| OS \ Compiler | MSVC | Clang | GCC |
|:-----------------:|:------:|:-------:|:-----:|
| Windows(latest)           | 19   | 21   | - | 
| Ubuntu(latest)           | - | 21   | 14 | 
| MacOS(latest)           | - | 21   | 14 | 

<a name="showcase2"></a> 
### Showcase 2: *Any++* with Model Map
```cpp
// <!--
#endif
// -->
namespace showcase2 {
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

struct circle {};
struct square {
  [[nodiscard]] std::string edgy_salute() const { return "edgy World"; }
};

TRAIT(drawable, (ANY_FN(std::string, draw, (), const)))

ANY_MODEL_MAP((circle), drawable) {
  static std::string draw(circle const&) { return "Silent greetings"; };
};

ANY_MODEL_MAP((square), drawable) {
  static std::string draw(square const& self) { return self.edgy_salute(); };
};

void draw(std::stringstream& os,
          std::vector<anyxx::any<anyxx::val, drawable>> const& drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

TEST_CASE("Showcase2") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Silent greetings\nedgy World\n");
}
};  // namespace showcase2
// <!--
#if 0
// -->
```
Showcase 2 demonstrates, how to use the ``Any++`` library's "model map" feature 
to provide custom behavior for different types using traits, whitch have nothing in common.

[Compiler Explorer] **TODO**

<a name="showcase3"></a> 
### Showcase 3: *Any++* Open Multi Dispatch, (Type Erased Binary Operator)
```cpp
// <!--
#endif
// -->
#if !defined(__clang__)  // clang makes no rewrite for our <=> operator :-(
                         // gcc and msvc do!
namespace showcase3 {
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <compare>
#include <string>

namespace ayx = anyxx;

struct circle {
  [[nodiscard]] std::string name() const { return "circle"; }
};
struct square {
  [[nodiscard]] std::string name() const { return "square"; }
};

struct figure_has_open_dispatch {};
ANY(figure, (ANY_FN(std::string, name, (), const)), ayx::cref)

anyxx::dispatch<std::partial_ordering(ayx::virtual_<any_figure<>>,
                                      ayx::virtual_<any_figure<>>)>
    comapare_edges;

[[nodiscard]] std::partial_ordering operator<=>(any_figure<> const& l,
                                                any_figure<> const& r) {
  return comapare_edges(l, r);
}

auto __ = comapare_edges.define<circle, circle>(
    [](auto const&, auto const&) { return std::partial_ordering::equivalent; });
auto __ = comapare_edges.define<circle, square>(
    [](auto const&, auto const&) { return std::partial_ordering::less; });
auto __ = comapare_edges.define<square, square>(
    [](auto const&, auto const&) { return std::partial_ordering::equivalent; });
auto __ = comapare_edges.define<square, circle>(
    [](auto const&, auto const&) { return std::partial_ordering::greater; });

void compare_each(std::stringstream& os,
                  std::vector<anyxx::any<anyxx::val, figure>> const& figures) {
  std::string sep;
  for (auto const& l : figures)
    for (auto const& r : figures) {
      os << std::exchange(sep, ", ") << l.name() << " ";
      if (l == r)
        os << "==";
      else if (l < r)
        os << "<";
      else
        os << ">";
      os << " " << r.name();
    }
}

TEST_CASE("Showcase3") {
  std::stringstream ss;
  compare_each(ss, {circle{}, square{}});
  CHECK(ss.str() ==
        "circle == circle, circle < square, square > circle, square == square");
}
};  // namespace showcase3
#endif  // __clang__
// <!--
#if 0
// -->
```
Showcase 3 demonstrates how to use the Any++ library to implement open multi-dispatch (type-erased binary operators) in C++. In this example:
- Two types, circle and square, are defined, each with a name() method.
- A figure trait is declared using the Any++ macro system, specifying a name() function.
- The dispatch<R(Args...)> mechanism is used to define a type-erased, runtime-resolved binary operator (operator<=>) for comparing two any_figure<> objects.
- The dispatch table is populated with custom comparison logic for each pair of types (circle vs circle, circle vs square, etc.), returning the appropriate std::partial_ordering result.
- The compare_each function iterates over all pairs of figures, compares them using the type-erased operator, and outputs the results.

[Compiler Explorer] **TODO**


<a name="showcase4"></a> 
### Showcase 4: *Any++* Open Dispatch As Visiitor
```cpp
// <!--
#endif
// -->
namespace showcase4 {
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace ayx = anyxx;

struct circle {
  [[nodiscard]] std::string name() const { return "circle"; }
};
struct square {
  [[nodiscard]] std::string name() const { return "square"; }
};

struct figure_has_open_dispatch {};
ANY(figure, (ANY_FN(std::string, name, (), const)), ayx::cref)

anyxx::dispatch<std::string(ayx::virtual_<any_figure<>>)> latin, italian;

auto __ = latin.define<circle>([](auto const&) { return "orbis"; });
auto __ = latin.define<square>([](auto const&) { return "quadratum"; });
auto __ = italian.define<circle>([](auto const&) { return "cerchio"; });
auto __ = italian.define<square>([](auto const&) { return "quadrato"; });

void translate(std::stringstream& os,
               std::vector<anyxx::any<anyxx::val, figure>> const& figures) {
  std::string sep;
  for (auto const& f : figures)
    os << std::exchange(sep, "; ") << f.name() << ": latin = " << latin(f)
       << ", italian = " << italian(f);
}

TEST_CASE("Showcase4") {
  std::stringstream ss;
  translate(ss, {circle{}, square{}});
  CHECK(ss.str() ==
        "circle: latin = orbis, italian = cerchio; square: latin = quadratum, "
        "italian = quadrato");
}
};  // namespace showcase4
// <!--
#if 0
// -->
```
Showcase 4 demonstrates how to use the Any++ library to implement open dispatch in the style of a visitor pattern, enabling runtime selection of behavior for different types without inheritance or virtual functions.
- Two types, circle and square, each provide a name() method.
- A figure trait is defined, specifying the required interface.
- Two open dispatchers, latin and italian, are created using dispatch<R(Args...)>, each mapping a figure to a localized string.
- The dispatchers are populated with type-specific translations for circle and square.
- The translate function iterates over a collection of type-erased figures, using the dispatchers to output the name and its translation in both Latin and Italian.
This is the so called "Open Visitor Pattern" implemented via open dispatch O(1) runtime complexity **without boilerplate**.

[Compiler Explorer] **TODO**

<a name="showcase5"></a> 
### Showcase 5: *Any++* Crosscast + Factory = Serialization
```cpp
// <!--
#endif
// -->
namespace showcase5 {
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace ayx = anyxx;

struct circle {
  double radius = 0.0;
  [[nodiscard]] double area() const { return radius * radius * 3.14; }
};
struct square {
  [[nodiscard]] double area() const { return edge_length * edge_length; }
  double edge_length = 0.0;
};

struct figure_has_open_dispatch {};
ANY(figure, (ANY_FN(double, area, (), const)), ayx::val)
ANY(serializeable, (ANY_FN(void, serialize, (std::ostream&), const)), ayx::val)

ayx::factory<any_serializeable, std::string, std::istream&> deserialize;

[[nodiscard]] any_figure<> deserialize_any_figure(std::istream& archive) {
  std::string type;
  archive >> type;
  return ayx::move_to<any_figure<>>(
      deserialize.construct<ayx::val>(type, archive));
}

ANY_MODEL_MAP((circle), serializeable) {
  static void serialize(circle const& self, std::ostream& os) {
    os << "circle " << self.radius << " ";
  };
};
auto __ = deserialize.register_("circle", [](std::istream& archive) -> circle {
  circle c;
  archive >> c.radius;
  return c;
});
ANY_REGISTER_MODEL(circle, figure);
ANY_REGISTER_MODEL(circle, serializeable);

ANY_MODEL_MAP((square), serializeable) {
  static void serialize(square const& self, std::ostream& os) {
    os << "square " << self.edge_length << " ";
  };
};
auto __ = deserialize.register_("square", [](std::istream& archive) {
  square s;
  archive >> s.edge_length;
  return s;
});
ANY_REGISTER_MODEL(square, figure);
ANY_REGISTER_MODEL(square, serializeable);

void areas(std::stringstream& os,
           std::vector<anyxx::any<anyxx::val, figure>> const& figures) {
  std::string sep;
  for (auto const& f : figures) os << std::exchange(sep, ", ") << f.area();
}

TEST_CASE("Showcase5") {
  std::stringstream archive{": circle 1 : square 2 : circle 3 : circle 4 end"};
  std::vector<any_figure<>> figures;
  std::string more;
  for (archive >> more; more != "end"; archive >> more)
    figures.push_back(deserialize_any_figure(archive));

  std::stringstream ss;
  areas(ss, figures);
  CHECK(ss.str() == "3.14, 4, 28.26, 50.24");

  std::stringstream serialized;
  for (auto const& f : figures) {
    serialized << ": ";
    anyxx::borrow_as<any_serializeable<anyxx::cref>>(f)->serialize(serialized);
  }
  serialized << "end";

  CHECK(serialized.str() == archive.str());
}
};  // namespace showcase5
// <!--
#if 0
// -->
```
Showcase 5 demonstrates how to use the Any++ library to combine cross-casting and factory patterns for serialization and deserialization of type-erased objects.
- Two types, circle and square, are defined, each with an area() method and serializable state (radius or edge_length).
- Two traits are declared: figure (with an area() method) and serializeable (with a serialize(std::ostream&) method).
- A factory (deserialize) is created to construct type-erased any_serializeable objects from a type name and an input stream.
- Model maps provide custom serialization logic for each type.
- The deserialize_any_figure function reads a type name from the stream, uses the factory to construct the correct type, and cross-casts it to a figure.
- The test case deserializes a sequence of shapes from a stream, computes their areas, serializes them back, and checks that the serialization matches the original input.
Summary:
This example shows how Any++ enables runtime type selection, safe cross-casting between interfaces, and pluggable serialization logic for unrelated types, all using type-erased objects and open extension points.

[Compiler Explorer] **TODO**

<a name="showcase6"></a> 
### Showcase 6: Basic *Any++* std::variant usage
```cpp
// <!--
#endif
// -->
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace showcase6 {
struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

TRAIT(drawable, (ANY_FN(std::string, draw, (), const)))

using known_shapes = std::variant<circle, square>;

void draw(std::stringstream& os,
          std::vector<anyxx::any<anyxx::by_val<known_shapes>, drawable>> const&
              drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

TEST_CASE("Showcase6") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Hello\nWorld\n");
}
}  // namespace showcase6
// <!--
#if 0
// -->
```
Showcase 6 demonstrates how to use the Any++ library with `std::variant` to enable type-erased polymorphism over a fixed set of types.
- Two types, `circle` and `square`, are defined, each with a `draw()` method returning a string.
- A `drawable` trait is declared, specifying the required interface (`draw() const -> std::string`).
- A `known_shapes` type alias is defined as `std::variant<circle, square>`, representing a closed set of possible shapes.
- The function `draw` takes a vector of type-erased objects (`anyxx::any<anyxx::by_val<known_shapes>, drawable>`) and calls `draw()` on each, writing the results to a stringstream.
- The Catch2 test verifies that drawing a `circle` and a `square` produces the expected output.
This example shows how Any++ can be combined with `std::variant` to provide type-erased, trait-based polymorphism for a known set of types, allowing heterogeneous collections and uniform interface access without inheritance or virtual functions.
Because the set of types is fixed, this approach can offer better performance than fully dynamic type erasure while still providing flexibility and extensibility through traits.

[Compiler Explorer] **TODO**

<a name="showcase7"></a> 
### Showcase 7: Basic *Any++* open std::variant usage: 'vany'
```cpp
// <!--
#endif
// -->
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace showcase7 {
namespace ayx = anyxx;
using namespace std::string_literals;

struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

ANY(figure, (ANY_FN(std::string, draw, (), const)), ayx::val)

using known_and_unknown_shapes =
    anyxx::make_vany<any_figure, ayx::val, circle, square>;
static_assert(
    std::same_as<known_and_unknown_shapes,
                 any_figure<ayx::by_val<  // see the Any++ logo at the top
                     std::variant<any_figure<ayx::val>, circle, square>>>>);
ANY_MODEL_MAP((std::string), figure) {
  static std::string draw(std::string const& s) { return s; };
};

void draw(std::stringstream& os,
          std::vector<known_and_unknown_shapes> const& figures) {
  for (auto const& f : figures) os << f.draw() << "\n";
}

TEST_CASE("Showcase7") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}, any_figure<>{"The big unknown..."s}});
  CHECK(ss.str() == "Hello\nWorld\nThe big unknown...\n");
}
}  // namespace showcase7
// <!--
#if 0
// -->
```
Showcase 7 demonstrates how to use the Any++ library to combine open type erasure with `std::variant` for extensible,
heterogeneous collections. Let us call them "vany" (variant-any).
- Two types, `circle` and `square`, are defined, each with a `draw()` method returning a string.
- A `figure` trait is declared, specifying the required interface (`draw() const -> std::string`).
- The type alias `known_and_unknown_shapes` is created using `anyxx::make_vany`, which produces a type-erased wrapper over a `std::variant` containing:
  - All known types (`circle`, `square`)
  - An open-ended type-erased fallback (`any_figure<ayx::val>`)
- A model map is provided for `std::string`, allowing strings to be handled as figures.
- So the vector passed to ``draw`` can seamlessly contain both known types (like circle and square) and dynamically extended types (like std::string), all accessed through the same trait-based interface. 

This example shows how Any++ enables a hybrid approach: you get the performance of `std::variant` for known types, 
while still supporting open-ended extension with type-erased objects. This is useful for scenarios where you want 
to handle a fixed set of types efficiently, but also allow for runtime extension or plugin types, all through a uniform trait-based interface.

"vany" is a modern, safer, and extensible ~~variant~~ version of OLEVariant and QVariant.

[Compiler Explorer] **TODO**


If you are still here, you are ready for [more Examples](https://www.alexweb.io/anyxx/examples.html)
<!--
#endif
// -->
