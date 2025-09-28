---
title: As we came on the dark side of type erasure
parent: blog
nav_order: 1
---

# As we came on the dark side of type erasure

This is a short story, with a lot of code, to tell from exitment, illusion, realisation and new orientation around "type erasure"

<img width="920" alt="image" src="https://github.com/user-attachments/assets/65f7f175-08d6-4c5e-b3e1-7cb45a59779b">

The story starts years back, by watching [Sean Parent talking about type erasure](https://www.youtube.com/watch?v=_BpMYeUFXv8).
This was for us, as for many others, a really empowering expirience.
It covered so many topics. For now, we will concentrate on the "type erasure" part.

### A short recap 

Let us start with a short recap of the quintesence in regard to "type erasure".
To eliminate the boilerplate code, we use ["proxy"](https://github.com/microsoft/proxy). "proxy" is the "type erasure" library roposed for inclusion in c++26.

The sample uses only a small part of the many features available in this awesome library.

```c++
#include <iostream>
#include <vector>
#include "https://raw.githubusercontent.com/microsoft/proxy/refs/heads/main/proxy.h"

struct position { float x, y; };

struct circle {
    double radius;
    void draw(position p) const {
        std::cout << " A Circle Is Recorded At " << p.x << " " << p.y << ", Area: " << area() <<  std::endl;
    }
    double area() const {  
      return radius * radius * 3.14;
    }
};
struct rectangle {
    int w, h;
    void draw(position p) const {
        std::cout << " A Rectangle Is Recorded At " << p.x << " " << p.y << ", Area: " << area() << std::endl;
    }
    double area() const {
        return w * h;
    }
};

PRO_DEF_MEM_DISPATCH(draw_convention, draw);
struct drawable : pro::facade_builder
    ::add_convention<draw_convention, void(position) const>
    ::support_copy<pro::constraint_level::nontrivial>
    ::build {};

void draw(const std::vector<pro::proxy<drawable>>& drawables){
    position pos{ 1.0, 2.0 };
    for( auto drawable: drawables )
        drawable->draw( pos );
}

int main()
{
    circle c{ 1.0,};
    rectangle r{3, 4};
    draw({ {&c}, {&r} });
    return 0;
}
```
[see it on compiler explorer]: https://en.wikipedia.org/wiki/Expression_problem

Some objects are constructed in "main".

Their addresses are add to a vector with elememnts of type pro::proxy<drawable>.

This is the "type eraser".

The magic happens then inside of the algorithm. 

The "type erasure run time dispatch", provided by proxy, takes care, that the corresponding function of the concrete object is executed.

Wes see, that 
- the implementation of the interface ("drawable") used by the algorithm is not tied to the implementation of the conrete type ("circle", "spare", .. ), 

and

- the implementation of the interface itself can be very generic by use of clever template tricks.

These features are intruding.

So it is understanding, that "type erasure" is the new cool thing in regards to get_meta_data dispatch. 

This reaches so far, that new languages, like "rust" go full in on that concept, and dissmiss the idea of inherritance as a whole.

The key messeage we get told is: Programming along classes utilizing the conventional v-table is old school and outdated.
(see "type erasure ["My existing project uses virtual functions. How should I migrate to “Proxy”?]: https://microsoft.github.io/proxy/docs/faq.html#how-migrate)

And here ends the the usual story. 

### A practical application 

We where convinced too. 
So we tried to apply this techniqe to an aspect of our codebase.  
Simplified, we had this structure

```c++
class Base {
public:
  std::string Value() const;
private: // data
};

class Derived : Base {
public:
  std::string Value() override;
  int Scope();
private: // more data
};
```

We wanted to came up mit soemthing like, this:
```c++
class Base {
public:
  std::string ToString() const;
private: // data
};

class Derived : Base {
public:
  int Value();
private: // more data
};

class DerivedLigthweight {
public:
  std::string Value();
  int Scope();
private: // nearly no data
};
```
"Derived" needed two distinct implementations, becuause in many important cases, tho original "Derived" is too heavy.

In terms of "proxy", we wanted something like this:
```c++
#include <iostream>
#include <vector>
#include "https://raw.githubusercontent.com/microsoft/proxy/refs/heads/main/proxy.h"

struct Base {
  std::string b_data_;
  std::string Value() const { return b_data_; }
};
struct Derived : Base {
  int d_data_ = 0;
  int Scope() const { return d_data_; }
};
struct DereivedLigthweight {
  std::string::value_type lw_data = 'a'; // nearly no data
  std::string Value() const { return std::string( 21, lw_data ); }
  int Scope() const { return (int)lw_data * 31; }
};

PRO_DEF_MEM_DISPATCH(ValueMem, Value);
PRO_DEF_MEM_DISPATCH(ScopeMem, Scope);
struct HasValue : pro::facade_builder
  ::add_convention<ValueMem, std::string() const>
  ::support_copy<pro::constraint_level::nontrivial>
  ::build {};
struct HasValueAndScope : pro::facade_builder
  ::add_facade<HasValue>
  ::add_convention<ScopeMem, int() const>
  ::support_copy<pro::constraint_level::nontrivial>
  ::build {};

void function_a(pro::proxy<HasValue> i) { std::cout << i->Value() << std::endl; }
void function_b(pro::proxy<HasValueAndScope> i) { std::cout << i->Value() << ", " << i->Scope() << std::endl; }

int main()
{
  Base base{ "base" };
  Derived derived{ "derived", 4711 };
  DereivedLigthweight dereivedLigthweight{ 'x' };

  function_a(&base);
  function_a(&derived);
  function_a(&dereivedLigthweight);

  function_b(&derived);
  function_b(&dereivedLigthweight);

  return 0;
}
```
[see it on compiler explorer]: https://godbolt.org/z/K5Y7GdW5Y

### Downcast? 

So far, so good. 

But but our functions behave not look like the one in the last example.
They are more like this:

```c++
base f1(pro::proxy<HasValue> a, pro::proxy<HasValue> b, std::function< bool(pro::proxy<HasValue>) > ) {
   // do somethin clever and then select the return type
   return {};
}
void f2(pro::proxy<HasValueAndScope> a, pro::proxy<HasValueAndScope> b ) {
  auto result_f1 = f1(a, b, [](auto x){ return downcast_to<pro::proxy<HasValueAndScope>>(x).Scope() > 30; });
  auto result = i_need_a_downcast_to<pro::proxy<HasValueAndScope>>(result_f1);
  std::cout << result.Scope() << "\n";
}
int main() {
  f2(DereivedLigthweight{'x'}, Derived derived{"derived", 4711});
  return 0;
}
```

The called functions
- filter the input with predicate callbacks
and
- they return results. 

But because we have **erased** away ALL **type** informatiom, we have no longer access to the data we need, to 
- answer the questions asked to the predicate functions via callbcks
and
- to continue with our processing, because we need the full interface we passed into the functions.

We found no solution to this pattern in ["proxy"](https://github.com/microsoft/proxy) and the other libraries we searched
- [Boost Type Erasure](https://www.boost.org/doc/libs/1_78_0/doc/html/boost_typeerasure/any.html#boost_typeerasure.any.conversions)
- [AnyAny](https://github.com/kelbon/AnyAny)
- [Dyno](https://github.com/ldionne/dyno)

So we resorted to a old school unsexy "OO-Style + template mixture" to solve this particular riddle.
if you are interested, look at a [scetch on compiler explorer]: https://godbolt.org/z/dPPzKzzEq. 
But beware, that looks realy ugly. It shows primary, there is a lot room for improvement.

### from "type erasure" to "type tunneling":

There is a pattern, that shows a general hole in the application of "type erasure".
We called that pattern the "type_erased_downcast problem".
This pattern can be reduced to this code lines.

```c++
// pseudo code!!!
maketype_erased< base > for { int func1() const; }; }
maketype_erased< derived : base > { int func2() const; };
struct S {
  int func1() const { return 1; };
  int func2() const { return 2; };
};
base f1( base b ) { return b; }
derived f2( derived d ) { return typerased_downcast< derived >(f1(d)); }
int main() {
  cout << f2( S{} ).func2() << "\n";
  return 0;
}
```

What we need, is a make_type_erased "thing" that supports "downcast".
So the quitessence, as we took it, is, that "type erasue" is not the end. We need kind of "type tunnel".
So the object can pass thru lower abstraction levels, with a fitting facade for them.
But when we get them back, we need to recover its ritcher interface or even its real type.

Before we came to an (partial) solution for this problem, we needed to take some other angles on it.
One of this perspectives came from std::any. 
We will show next time.

PS: We are no "Rust" experts. So we are curios, how this kind of pattern is solved there. As [we understand]: https://microsoft.github.io/rust-for-dotnet-devs/latest/language/custom-types/interfaces.html, 
"Rust" has no downcasting for "traits". Maybe the answer is simple "Rust programmer write better programs, so they do not run in this kind of quirx" ;-)
