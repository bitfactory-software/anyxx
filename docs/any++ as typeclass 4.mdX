\page subpage4 Chapter 4: Any++: From Type Erasure to Type Class 

Rust traits are not only a way to define behavior for abstract types, but also a way to achieve external runtime polymorphism, aka Type Erasure.
With that in mind, we recognized while developing Any++, that some of the mechanisms used to implement Type Erasure here can also be used for compile-time type class-like behavior.

Implementing Type Erasure requires three components:

- A "V-Table" for indirecting function calls.
- A "Facade" for the ergonomic connection between the data and the "V-Table."
- An "Adapter" to connect the functions of the "V-Table" to the specific type.

The basic idea for static dispatch is to replace the "V-Table" with a template based type dispatch.

Let us start with a simple example, an "equal_comparable" trait:
```C++
TRAIT(equal_comparable,
      (ANY_OP_DEF(public, bool, ==, eq, (anyxx::self const&), const,
                  [&x](auto const& r) {
                    return x == r;
                  }),
       ANY_OP_DEF(public, bool, !=, ne, (anyxx::self const&), const,
                  [&x](auto const& r) {
                    return !(trait_as<equal_comparable>(x) ==
                             trait_as<equal_comparable>(r));
                  })))
```
The trait defines two operators, == and !=, and their implementations. Let's see how to use this:


