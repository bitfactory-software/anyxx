[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)

# any++: *type erasure* vocabulary  for *programming on a large scale*

This library is targeted to solve coupling problems and allow maximal seperation with reasonable runtime performance.

**Reasonable** means **on par with virtual functions**.

# Showcase 1: Basic *ANY* usage
```cpp
#include <bit_factory/anyxx.hpp>
#include <iostream>

struct circle {
    void draw(std::ostream& os) const { os << "Circle\n"; }
};
struct square {
    void draw(std::ostream& os) const { os << "Square\n"; }
};

ANY(any_drawable, (ANY_METHOD(void, draw, (std::ostream&), const)))

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

# Feature overview
- **External Polymorphis**m via *ANY...* metaclass
  - Nonintrusive Runtime Polymorphism, aka "type erased interfaces", "dynamic interfaces", "traits"
    - Deriveable
    - Operators
    - Choose implementation for specific type va *Concept Map*
    - Upcast, **Downcast**, **Crosscast*** via *borrow_as*, *clone_to*, *move_to*
   - **Interface** can be **template**
     - 'recursive' to erase container
   - **open dispatch** with **O(1) runtime complexity**
     - **single** (visitor)
     - **multiple** (collision resolver, type erased binary operators)  
     - performance on par with vanilla virtual functions
  - **Type erased clone**, if concrete type is *copy constructable*
- **Abstract factory** for *ANY*s 
- **Transparent storeage/lifetime management***
  - Out of the box:
    - const/mutable_observers, aka "pointer"
    - shared_const, weak via std::shared_ptr, std::week_ptr
    - unique, via std::unique_ptr and type erased deleter
    - value
  - Customizable via anyxx::trait
  - **Safe** interchange of **lifetime**  
    - (implicit) **borrow_as**: value -> ..._observer, unique -> ..._observer, shared_const -> const_observer, mutable_observer -> const_observer
    - **move_to**: value -> unique -> shared_const, unique -> value
    - **clone_to**: ..._observer -> shared_const -> unique -> value
- **Extension member*
  - To **add members at load time**
  - Has access performance on par with *virtual* function getter
- **Hook**
  - A overrideable **customization point  **     
- Single **header only library**
- **static-** or **DLL/SO mode** for v-tables, dispatch tables and other static meta data.
  - Default for *static build*: the compiler manages the details.
  - ``#define ANYXX_DLL_MODE``, works for *static* and *dynamic builds*`: You control the visibility and location of the static meta data.

# Useage in CMakeLists.txt:
```
FetchContent_Declare(
    cogoproject
    GIT_REPOSITORY https://github.com/bitfactory-software/anyxx.git
    GIT_TAG main
)
FetchContent_MakeAvailable(anyxx)
```
# Performace compared
| Benchmark     | 12th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>MS Visual C++ 18.0.1 /O2 /Ob2 | AMD Ryzen 9<br> 5900X 12-Core Processor (3.70 GHz)<br>MS Visual C++ 17.14.14 /O2 /Ob2 | 2th Gen Intel(R)<br>Core(TM) i12900H (2.50 GHz)<br>clang-cl /O2 /Ob2 |
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

# CI Matrix
| OS \ Compiler | MSVC | Clang | GCC |
|:-----------------:|:------:|:-------:|:-----:|
| Windows(latest)           | 19   | 21   | - | 
| Ubuntu(latest)           | - | 21   | 14 | 
| MacOS(latest)           | - | 21   | 14 | 

# Showcase 2: Type erased *spaceship operator*




- To hide the representation with lifetime handles: [Quick start](/tutorials/tutorial___1.md/#t1) [Overview](docs/erased_data_overview.md)
- To hide the functionality with: [Overview](docs/erased_functionality.md)
    - Interfaces: [Quick start](/tutorials/tutorial__30.md) [Operators](/tutorials/tutorial__33.md/#t1)
        - Derivable [tutorial](/tutorials/tutorial__31.md)
        - Upcastable [tutorial](/tutorials/tutorial__31.md)
        - Downcastable [tutorial](/tutorials/tutorial__31.md)
    - Open methods: [Quick start](/tutorials/tutorial___1.md/#t2)
- Allow extendable types with open_objects: [Quick start](/tutorials/tutorial___2.md)
- To obtain types with get_meta_data casts

To enable:

- Loose coupling and
- Performance on par with virtual function calls.

They work as a foundational vocabulary to *factor out* the dependencies between the components in the system.

The library is implemented in C++20. It can be used as a static or as a dynamic library.

The name of the library originates from its fundamental design idea:

- **Hide** the concrete **data** behind a (possibly *smart*) **pointer to void**,
- Pair this with a pointer to **static meta** data to interpret and **recover** the hidden data
- Use such pairs to **connect the concrete data** via **interfaces** and **open methods**

to separate usage and implementation details as much as possible.

This makes the pointer seem not *void* but rather **virtual void**.

## Programming on a large scale

### *Runtime* versus *compile-time* **dispatch**

[Dispatch](medium.com/ingeniouslysimple/static-and-dynamic-dispatch-324d3dc890a3) on behalf of a type is - like function calls - a fundamental software construction principle to allow the building of layered abstraction levels.

Static dispatch is the dispatch of choice in C++. Foremost because of performance and binary size.

But once a program exceeds a certain size, it is no longer possible to use static dispatch for everything.

Reasons are:
- You do not know all your types all the time
- You cannot use all your types all the time because of restricted build resources: time and space.

For example, imagine a std::variant for some hundred different alternatives.
This is where **programming on a large scale** begins and get_meta_data dispatch is the proven means.

At this point arises another problem, when it comes to "inversion of control" in regard to the type of the parameter objects passed into the callback:
- In static dispatch, you have the full type information.
- In dynamic dispatch, the original type is usually abstracted away (either via a any_base class or "type erasure") and must be cast back to access more concrete information.

### *Runtime* versus *compile-time*

If you have a get_meta_data performance problem, the hot get_meta_data path and the causing bottleneck are easily diagnosed with profilers.
When the problem is spotted, the solution is - in our experience - not far away.
In contrast, when build time has grown to a problem, there is seldom an easy way out.
While a software system grows larger, keeping the edit-compile-run loop short requires constant smart decisions.
The fundamental design principles here is **physical loose-coupling** by reducing the compiltime dependencies between the compnents - functions and types - in the programm.


### *Logical* versus *physical* coupling

A C++ `template` allows a **loose coupled logical design**, because the client can be **any** `typename`. 
It requests, in contrast, a **strong physical coupling** between the client and the template, because the serving template must be fully known on the client side.

The interception of this cascading coupling is the aim of this library.

### From *OO* to *type erasure* to *type tunneling*

C++'s virtual functions have shown their versatility and usefulness for decades.
But, as the understanding of decomposing software grows, we see the "OO style" integration of the any directly into the object as a problem.

It is now consensus that separation of the any from the underlying object is the way to go. The type gets "erased" from the any.
On the contrary, by doing so, we lose this connection, which had shown its usefulness in countless huge and successful software projects.

A v-table any of an object can be used:
- To cast back to that object itself
- To "downcast" the any to a more specific one, and
- To "crosscast" to another any.

A vocabulary for programming on a large scale must support those casts.
Because we found no library that fulfilled these demands, we came up with this one.

We call a *lossless type erasing* technique **type tunneling**.
To symbolize the "hide the type information" and bring it "back to light."

### Coupling and Separation

Tight coupling allows information to flow easily and enhances productivity. However, if the "things" coupled are fast-moving, the coupling becomes a burden.
So we want to separate the moving parts.

***Examples***:

You will, for sure, couple with the used programming language as deeply as possible (as long as the language promises backward compatibility) to exploit its benefits.

You probably want to separate from the details of the operating system for easier portability.

You surely also want to separate different aspects of your application because you know they will change, and you want to keep the changes as local as possible.

For the decision over using this library, one has to balance the benefits of separating internal dependencies versus the tight coupling into **virtual void**.
