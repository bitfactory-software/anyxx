[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)

# any++

This library is targeted to solve coupling problems and allow maximal **static** and **dynamic seperation**.

The performance of **dynamic dispatch is on par with virtual functions**.

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
          std::vector<any_drawable<anyxx::shared_const>> const& drawables) {
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

### Feature overview
- **External Polymorphism** via *ANY...* metaclasses
  - Nonintrusive **Runtime** ***AND*** **Static** Polymorphism, aka "type erased interfaces", "dynamic interfaces", "traits"
    - Deriveable
    - Operators
    - Choose implementation for specific type va *Concept Map*
    - Upcast, **Downcast**, **Crosscast** via *borrow_as*, *clone_to*(if concrete type is *copy constructable*), *move_to*
   - **Interface** can be **template**
     - 'recursive' to erase container
   - **open runtime dispatch** with **O(1) runtime complexity**
     - **single** (visitor)
     - **multiple** (collision resolver, type erased binary operators)  
     - performance on par with vanilla virtual functions
- **Abstract factory** for *ANY*s 
- **Transparent storeage/lifetime management***
  - Out of the box:
    - const/mutable_observers, aka "pointer"
    - shared_const, weak via std::shared_ptr, std::week_ptr
    - unique, via std::unique_ptr and type erased deleter
    - value
    - by_val
    - vany variant
  - Customizable via anyxx::erased_data_trait
  - **Safe** interchange of **lifetime**  
    - (implicit) **borrow_as**: value -> ..._observer, unique -> ..._observer, shared_const -> const_observer, mutable_observer -> const_observer
    - **move_to**: value -> unique -> shared_const, unique -> value
    - **clone_to**: ..._observer -> shared_const -> unique -> value
- **Extension member*
  - To **add members at load time**
  - Has access performance on par with *virtual* function getter
- **Hook**
  - A overrideable **customization point**     
- Single **header only library**
- **static-** or **DLL/SO mode** for v-tables, dispatch tables and other static meta data.
  - Default for *static build*: the compiler manages the details via *static inline*.
  - ``#define ANYXX_DLL_MODE``, works for *static* and *dynamic builds*`: You control the visibility and location of the static meta data.

### architectural overview
<img width="2695" height="1192" alt="anyxx architecture" src="https://github.com/user-attachments/assets/b6ae44bf-c78f-4b4f-83cf-201c6931ec98" />

### Useage in CMakeLists.txt:
```
FetchContent_Declare(
    cogoproject
    GIT_REPOSITORY https://github.com/bitfactory-software/anyxx.git
    GIT_TAG main
)
FetchContent_MakeAvailable(anyxx)
```
### Performace compared
| Benchmark     | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>MS Visual C++ 18.0.1 /O2 /Ob2 | AMD Ryzen 9<br> 5900X 12-Core Processor (3.70 GHz)<br>MS Visual C++ 17.14.14 /O2 /Ob2 | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>clang-cl /O2 /Ob2 |
|:----------------------------|--------------:|-------:|-------:|
| **Single dispatch** |   | |
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

