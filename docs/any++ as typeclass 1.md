\page subpage1 C++0x Concepts, Haskell type classes, Rust traits, a short introduction

In 2009 the Standard Committee has been working on C++0x Concepts for a long time, and they are finally getting close to a release.
The goal was ambitious: To add a new language feature that allows to check template type parameters we "normal" function arguments.
It ment a way to define "concepts", which are sets of requirements that a type must satisfy.
With this feature, erros would be detected at the point of template instantiation, if the caller made a misttake, or inside the template, if the implementer used the parameters wrong.
The poroposal included concept maps, which are a way to define how a type fits to concept, and to provide default implementations for the requirements of a concept.
All this was too ambitious, and the quest for Concepts in C++0x was abandoned in July 2009. 

With C++20 we got Concepts Lite, which is a much simpler version of the original proposal. It allows to define concepts, but it does not include concept maps or default implementations. 
It is still a useful feature, but it is not as powerful as the original proposal. While the original proposal resembled to Haskell type classes, the current version concentrates on 
checking template parameters at compile time for overloading and providing better error messages.

In meantime, Rust has been developed with traits, which are a way to define behavior for abstract types. They are similar to Haskell type classes and C++0x Concepts, 
and are fare more powerful than C++20 Concepts Lite. 
They allow to define default implementations for the requirements of a trait, and they also support associated types, which are a way to define types that are related to the trait.
As with C++0x Concept Maps, you can define how a type fits to a trait.

In contrast to C++0x Concepts, Rust traits are not only a way to check and adapt types at compile time. Every trait can also be used as a type, 
which is a way to achieve external runtime polymorphism, aka Type Erasure. This is one of the most powerful features of Rust. 

C++ has some advanced Type Erasure libraries, but the compile time aspect of type classes is unadddressed.
This leaves us with a gap between C++ and Rust traits, which is the ability to 
- check template parameters at the implementation site
- provide default implementations for a concept/type class/trait.
- provide a way to define how a type fits to a concept/type class/trait

Any++ can offers a complete solutions for the last points, and a partial solution for the first. 
We will see in the next sections how Any++ can be used to achieve this.

But before we dive into the details, let's take a look at the \ref subpage2 "\"Hello World\" of type classes, the \"Monoid\"".

