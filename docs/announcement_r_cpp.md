# How to ``trait`` ``any`` ``virtual``, ``static`` or ``variant``?
# Any++: Rust-like traits for C++ with flexible type erasure (std::any + traits + open dispatch)

Hey r/cpp! I'd like to share **Any++**, a library that extends the `std::any` concept with Rust-like traits and open dispatch.

**Key Features:**
- **Flexible storage**: Type-erased, concrete types, or `std::variant` - your choice
- **Trait-based interfaces**: Add functions and operators to unrelated types without inheritance
- **Open multi-dispatch**: Type-erased double dispatch with O(1) runtime complexity
- **Performance**: On par with virtual functions (see benchmarks below)
- **Runtime-casting**: Runtime downcasts and cross-casts between different interfaces

**Quick Example:**
```
#include <iostream>
#include <string>
#include <bit_factory/anyxx.hpp>

struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

TRAIT(drawable, (ANY_FN(std::string, draw, (), const)))

void draw(std::ostream& os,
          std::vector<anyxx::any<anyxx::val, drawable>> const& drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

int main() {
  draw(std::cout, {circle{}, square{}});
  return 0;
}
```
- [Live on Compiler Explorer](https://godbolt.org/z/nGjjs9xva)

**Why use it?**
- Decouple interfaces from implementations (no inheritance needed)
- Add polymorphic behavior to third-party types
- Combine `std::variant` performance with type-erasure flexibility
- Implement visitor pattern without boilerplate

**Performance vs Virtual Functions:**
- Single dispatch: 1x (identical)
- Double dispatch: 1x (vs hand-rolled virtual functions)
- Using `std::variant` dispatch for known types in hot paths!

**Links:**
- [GitHub](https://github.com/bitfactory-software/anyxx)
- [Documentation](https://www.alexweb.io/anyxx/)
- Available on vcpkg: `vcpkg install anyxx`

The library is MIT licensed, header-only, and requires C++23.
Prerelease version! Expect future API changes!

Would love to hear your thoughts and use cases!