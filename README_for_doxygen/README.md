// <!--
// The MIT License (MIT)
//
// Copyright (c) 2024 Kris Jusiak <kris@jusiak.net>
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
[Hello World!](#showcase1) / [Model Map](#showcase2) / [Type erased Spaceship](#showcase3) 

[![MIT Licence](http://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/license/mit)
[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)
[![Static Badge](https://img.shields.io/badge/dos-Any%2B%2B-blue)](https://www.alexweb.io/anyxx/)

#Any++ : How to ``trait`` ``any`` ``virtual``, ``static`` or ``variant``?

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
  std::string draw() const { return "Hello"; }
};
struct square {
  std::string draw() const { return "World"; }
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
[Compiler Explorer] **TODO**

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
  std::string edgy_salute() const { return "edgy World"; }
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
[Compiler Explorer] **TODO**

<a name="showcase3"></a> 
### Showcase 3: *Any++* Open Multi Dispatch, (Type erased binary operator)
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
  std::string name() const { return "circle"; }
};
struct square {
  std::string name() const { return "square"; }
};

struct figure_has_open_dispatch {};
ANY(figure, (ANY_FN(std::string, name, (), const)), ayx::cref)

anyxx::dispatch<std::partial_ordering(ayx::virtual_<any_figure<>>,
                                      ayx::virtual_<any_figure<>>)>
    comapare_edges;

std::partial_ordering operator<=>(any_figure<> const& l,
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
[Compiler Explorer] **TODO**


If you are still here, you are ready for (more Examples)[https://www.alexweb.io/anyxx/examples.html]
<!--
#endif
// -->
