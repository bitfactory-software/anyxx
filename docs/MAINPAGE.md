<!--
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
// -->
[![MIT Licence](http://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/license/mit)
[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)
[![Static Badge](https://img.shields.io/badge/docs-Any%2B%2B-blue)](https://www.alexweb.io/anyxx/)

**Any++** enhances the well-known *boost::any* and *std::any* with:

- **Flexible choice of underlying storage mechanisms**
  - Type-erased
  - Concrete
  - Variant
- **Extensibility**
  - Ability to add functions and operators using Rust-like **traits**
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

**Special thanks go to [Alex](https://github.com/AlexCodesApps) for his lightweight and manageable preprocessor vocabulary shown in [dynamic_interface](https://github.com/AlexCodesApps/dynamic_interface#example).**

@tabs

@tab{Basic}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__1.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__1.cpp slide
@endtab
@endtabs

Showcase 1 demonstrates the most basic usage of `Any++` for type-erased 
polymorphism in C++. It defines two simple types, `circle` and `square`, each with a `draw()` method 
returning a string. Using the `Any++` macro system, a `drawable` trait is declared, 
specifying the required interface (`draw() const -> std::string`).

The draw function takes a vector of type-erased any objects that conform to the `drawable` trait.
It shows how to create and use a heterogeneous collection of unrelated objects (`circle` and `square`).

<img width="454" height="355" alt="Any++logo-small" src="https://github.com/user-attachments/assets/e4766dd4-7a21-486d-9dd0-8725149e0754" />

@endtab

@tab{Trait Map}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__2.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__2.cpp slide
@endtab
@endtabs

Showcase 2 demonstrates, how to use the `Any++` "model map" feature 
to provide custom behavior for unrelated types using traits.
@endtab

@tab{Type Erased Binary Operator}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__3.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__3.cpp slide
@endtab
@endtabs

Showcase 3 demonstrates how to use the Any++ library to implement open multi-dispatch (type-erased binary operators) in C++. In this example:
- Two types, `circle` and `square`, are defined, each with a `name()` method.
- A `figure` trait is declared using the `Any++` macro system, specifying a `name()` function.
- The `ANY` macro is an extension of the `TRAIT` macro. It defines the requested behavior (trait interface) and additionally creates a typedef for an `any` 
  type, named `any<first parameter of macro>`, that uses this trait. The generated trait itself has a template parameter for the Proxy type, 
  which is defaulted to the last parameter of the `ANY` macro.
- The `dispatch<R(Args...)>` mechanism is used to define a type-erased, runtime-resolved binary operator (`operator<=>`) for comparing two `any_figure<>` objects.
- The dispatch table is populated with custom comparison logic for each pair of types (`circle` vs `circle`, `circle` vs `square`, etc.) at static initialization time.
To do this with as little ceremony as possible, ``anyxx.hpp`` provides a ``__`` macro that simulates the[C++26 placeholder with no name](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2169r4.pdf).
That trick initializes a static variable with automatic name at each usage site with a dummy value, returned by ``declare`` only for this purpose.
- The `compare_each` function iterates over all pairs of figures, compares them using the type-erased operator, and outputs the results.
@endtab

@tab{No Visitor!}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__4.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__4.cpp slide
@endtab
@endtabs

Showcase 4 demonstrates how to use `Any++` to implement open dispatch in the style of a visitor pattern, enabling runtime selection of behavior for different types without inheritance or virtual functions.
- Two types, `circle` and `square`, each provide a `name()` method.
- A `figure` trait is defined, specifying the required interface.
- Two open dispatchers, `latin` and `italian`, are created using `dispatch<R(Args...)>`, each mapping a figure to a localized string.
- The dispatchers are populated with type-specific translations for `circle` and `square`.
- The `translate` function iterates over a collection of type-erased figures, using the dispatchers to output the name and its translation in both Latin and Italian.
This is the so-called "Open Visitor Pattern" implemented via open dispatch O(1) runtime complexity **without boilerplate**.
@endtab

@tab{Crosscast + Factory}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__5.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__5.cpp slide
@endtab
@endtabs

Showcase 5 demonstrates how to use `Any++` to combine cross-casting and factory patterns for serialization and deserialization of type-erased objects.
- Two types, `circle` and `square`, are defined, each with an `area()` method and serializable state (`radius` or `edge_length`).
- Two traits are declared: `figure` (with an `area()` method) and `serializable` (with a `serialize(std::ostream&)` method).
- A factory (`deserialize`) is created to construct type-erased `any_serializable` objects from a type name and an input stream.
- Model maps provide custom serialization logic for each type.
- The `deserialize_any_figure` function reads a type name from the stream, uses the factory to construct the correct type, and cross-casts it to a figure.
- The test case deserializes a sequence of shapes from a stream, computes their areas, serializes them back, and checks that the serialization matches the original input.
@endtab

@tab{std::variant}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__6.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__6.cpp slide
@endtab
@endtabs

Showcase 6 demonstrates how to use `Any++` with `std::variant` to enable type-erased polymorphism over a fixed set of types.
- Two types, `circle` and `square`, are defined, each with a `draw()` method returning a string.
- A `drawable` trait is declared, specifying the required interface (`draw() const -> std::string`).
- A `known_shapes` type alias is defined as `std::variant<circle, square>`, representing a closed set of possible shapes.
- The function `draw` takes a vector of objects, type-erased via std::variant (`anyxx::trait_as<known_shapes, drawable>`), and calls `draw()` on each, writing the results to a stringstream.
- The Catch2 test verifies that drawing a `circle` and a `square` produces the expected output.
This example shows how `Any++` can be combined with `std::variant` to provide type-erased, trait-based polymorphism for a known set of types, allowing heterogeneous collections and uniform interface access without inheritance or virtual functions.
Because the set of types is fixed, this approach can offer better performance than fully dynamic type erasure while still providing flexibility and extensibility through traits.
@endtab


@tab{vany}
@tabs
@tab{C++23}
  \snippet{talk_23__godbolt} Showcase_v23__7.cpp slide
@tab{C++26}
  \snippet{talk_26__godbolt} Showcase_v26__7.cpp slide
@endtab
@endtabs

Showcase 7 demonstrates how to use `Any++` to combine open type erasure with `std::variant` for extensible,
heterogeneous collections. Let us call them "vany" (variant-any).
- Two types, `circle` and `square`, are defined, each with a `draw()` method returning a string.
- A `figure` trait is declared, specifying the required interface (`draw() const -> std::string`).
- The type alias `known_and_unknown_shapes` is created using `anyxx::vany`, which produces a type-erased wrapper over a `std::variant` containing:
  - All known types (`circle`, `square`)
  - An open-ended type-erased fallback (`any_figure<ayx::val<>>`)
- A model map is provided for `std::string`, allowing strings to be handled as figures.
- So the vector passed to `draw` can seamlessly contain both known types (like `circle` and `square`) and dynamically extended types (like `std::string`), all accessed through the same trait-based interface. 

This example shows how `Any++` enables a hybrid approach: you get the performance of `std::variant` for known types, 
while still supporting open-ended extension with type-erased objects. This is useful for scenarios where you want 
to handle a fixed set of types efficiently, but also allow for runtime extension or plugin types, all through a uniform trait-based interface.

"vany" is a modern, safer, and extensible ~~variant~~ version of OLEVariant and QVariant.
@endtab

@endtabs

More Examples ☞ \ref talk_meeting_cpp26

### Available on vcpkg:

Install in vcpkg root directory:
```
vcpkg install anyxx
```

In your CMakeLists.txt:
```cmake
find_package(anyxx CONFIG REQUIRED)
target_link_libraries(main PRIVATE bit_factory::anyxx)
```

### Useage in CMakeLists.txt:
```
FetchContent_Declare(
    bit_factory::anyxx
    GIT_REPOSITORY https://github.com/bitfactory-software/anyxx
    GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(anyxx)
```


### Clone the repo:
```
git clone - c core.symlinks = true https://github.com/bitfactory-software/anyxx
```
### Runtime Performance compared
| Benchmark Invocation **Time** | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>MS Visual C++ 18.0.1 /O2 /Ob2 | AMD Ryzen 9<br> 5900X 12-Core Processor (3.70 GHz)<br>MS Visual C++ 18.2.1 /O2 /Ob2 | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>clang-cl /O2 /Ob2 | Source | Data from ... |
|:------------------------------|----------------------------------------------------------------------------------:|---------------------------------------------------------------------------:|-------:| ----------------------------------------------------------------------:|-----------------------:|
| **Single dispatch**           | --- | --- | --- | --- | --- |
| virtual function (**reference**) |                                                                              1 |                                                                          1 |        1 | [Source](https://github.com/bitfactory-software/anyxx/blob/master/examples/inline_mode/20_Tree_virtual_function.cpp) | ``BENCHMARK("20_Tree virtual function value") { return expr->value(); };`` |
| any++ interface |  x **1** | x **1** | x **1** | [Source](https://github.com/bitfactory-software/anyxx/blob/master/examples/inline_mode/21_Tree_any.cpp) | ``BENCHMARK("21_Tree any value") { return expr->value(); };`` |
| any++ open method ``dispatch``| x **1.5** | x **1.5** | x **1.5** | [Source](https://github.com/bitfactory-software/anyxx/blob/master/examples/inline_mode/21_Tree_any_dispatch.cpp.cpp) | ``BENCHMARK("21_Tree any dispatch value") { return expr->value(); };`` |
| **Double dispatch**                          | --- | --- | --- | --- | --- |
| hand rolled w. virtual function (**reference**) | 1 | 1 | 1 | [Source](https://github.com/bitfactory-software/anyxx/blob/master/examples/inline_mode/30b_Animals_virtual_functions.cpp) | ``BENCHMARK("30b_Animals virtual functions")`` |
| any++ open method ``dispatch`` | x **1** | x **1** | x **1** | [Source](https://github.com/bitfactory-software/anyxx/blob/master/examples/inline_mode/31_Animals_any_dispatch.cpp) | ``BENCHMARK("31_Animals any dispatch")`` |
| ``std::variant`` + ``std::visit``| x 0.8 | x 0.65 | x 0.35 | [Source](https://github.com/bitfactory-software/anyxx/blob/master/examples/inline_mode/30a_Animals_variant_visit.cpp) | ``BENCHMARK("30a_Animals variant visit")`` |


### CI Matrix
| OS \ Compiler | MSVC | Clang | GCC |
|:-----------------:|:------:|:-------:|:-----:|
| Windows(latest)           | 18   | 23   | - | 
| Ubuntu(latest)           | - | 23   | 16 | 
| MacOS(latest)           | - | 23   | 16 | 


