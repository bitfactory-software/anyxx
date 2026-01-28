[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)

# Any++ : How to *trait* *any* *virtual*, *static* or *variant*?

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

The combination of static and runtime storage enables a hybrid approach of `std::variant` and **type erasure** with a unified syntax and open double dispatch.

### Showcase 1: Basic *ANY* usage
```cpp
#include <bit_factory/anyxx.hpp>
#include <iostream>

struct circle {
    void draw(std::ostream& os) const { os << "Circle\n"; }
};
struct square {
    void draw(std::ostream& os) const { os << "Square\n"; }
};

ANY(any_drawable, (ANY_FN(void, draw, (std::ostream&), const)))

void draw(std::ostream& os,
          std::vector<any_drawable<anyxx::shared>> const& drawables) {
    for (auto const& drawable : drawables) drawable.draw(os);
}

int main() {
    draw(std::cout, {std::make_shared<circle>(), std::make_shared<square>()});
    return 0;
}
```
Output:
```
Circle
Square
```
[Compiler Explorer](https://godbolt.org/z/4P6M8WrzP)

### architectural overview
<img width="2695" height="1192" alt="anyxx architecture" src="https://github.com/user-attachments/assets/b6ae44bf-c78f-4b4f-83cf-201c6931ec98" />

### Useage in CMakeLists.txt:
```
FetchContent_Declare(
    bit_factory::anyxx
    GIT_REPOSITORY https://github.com/bitfactory-software/anyxx.git
    GIT_TAG main
)
FetchContent_MakeAvailable(anyxx)
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

