# Terms and Definitions in Context of This Library

### Erased Interface
An object whose members are function pointers, and every function has as **its first parameter *self***. This is a pointer to (eventually *const*) *void*.

### Implemented Erased Interface
#### An object derived from an interface, with no additional members, and where all members point to valid functions.

These functions are implemented via a templated constructor. The template parameter is called the **unerased** type.  
In these functions, the "self" parameter is cast back to a pointer to the unerased type, and the correct function for the "unerased" type will be called. The default for the called function is a member function with the same name and signature as the specified interface function. This behavior can be customized in a **v_table_map** for the unerased type. [Tutorial](/tutorials/tutorial___1.md/#t4)

### virtual_void
#### A concept describing an object that **erases** the **type** and the **lifetime** of **another** object.

It must deliver a *void* pointer of that object and a pointer to a *meta* object.  
Responsible for the description of such types is the specialized *virtual_void_trait*.  

The library offers these *lifetime* holders:
- **observer**: Takes no ownership. The creator of such an observer is responsible for ensuring that the referenced object outlives the observer. There are two flavors: *const* and *mutable*, for read-only or modifying access to the referenced object.
- **shared_const**: Ownership as `std::shared_ptr`. The delivered address is a pointer to *const void*. The *meta* is allocated along with the concrete data.
- **shared_ptr**: Pairs a `std::shared_ptr<void>` with the *meta*. Use instead of shared_const if you cannot control the construction of the object that you want to *erase*.
- **unique**: Ownership as `std::unique_ptr`. The delivered address is a pointer to a *mutable* object.
- **unique_ptr**: Pairs a `std::unique_ptr<void>` with the *meta*. Use instead of unique if you cannot control the construction of the object that you want to *erase*.
- **value**: Every value object holds its own copy. Same semantics as *int*. The delivered *void* pointer is *mutable*. [Tutorial](/tutorials/tutorial___1.md/#t1)

#### There are three kinds of meta objects in the library:
- *has_no_meta*
- *has_type_info*
- *has_m_table*: Has *type_info* and a pointer to an *m_table* for fast dispatch in an *open method*.

### 'virtual_void' Versus 'typed_void'
#### A *virtual_void* holds no compile-time information about the *held* object.

Only if *virtual_void* has *runtime meta-data*, is there a dynamic and safe cast of the *void* pointer to a concrete pointer possible.  
*virtual_void* has two kinds of such meta-data:
- **typeid**
- **m_table**

A *virtual_typed* is a typed wrapper atop *virtual_void*. If the *virtual_void* holds in its meta an *type_info*, the cast to *virtual_void* is a safe cast.

### Static Cast vs Dynamic Cast
#### A *static cast*:
  - Is in the language type rules and always checked by the compiler.
  - Upcasts are always safe and static.
  - Static downcasts are unsafe.
  - Static casts are only a syntactic construct and leave no trace in the binary code.

#### A *dynamic cast*:
  - Is a runtime query to test if the casted object is of this type.
  - If such a query succeeds, it is determined by the program.
  - All dynamic casts are safe.
  - Dynamic casts need code to run.

### 'Upcast' vs 'Downcast'
#### An *upcast* is a *conversion* from a more detailed type to a general one.

#### A *downcast* is a conversion from a more general type to a more detailed one.

For upcasts and downcasts, the types must be related within the language rules.  
Static downcasts are guesses and as such are unsafe. Dynamic downcasts are by definition safe and a kind of *type query*.

### Lifetime Cast
#### A *virtual_void* can be cast to another *virtual_void*, in the sense of an *upcast*.

- A **smart** *virtual_void* (**shared**, **unique**, **value**) can be cast to an **observer**.
- A **mutable** *virtual_void* can be cast to a *read-only observer*.

But not in the other direction.

### Crosscast
#### While *up-* and *downcasts* are within related types, *crosscasts* are between unrelated types.

Such types are typically virtual base types (an interface).  
A *crosscast* usually tests if one interface can be reached from another, and if so, provides access to it.

### Open Method
#### An **open method** is a freestanding callable, which acts like a virtual member function.

Open member functions are a recipe to solve the [expression problem]. An open method is the simplest but very useful case of [open multi-methods]. With an open method, you can add a function whose behavior is determined by the type of its (first) argument, but you do not need to change the definition of that type. [Tutorial](/tutorials/tutorial___1.md/#t2)

### Open Type
#### An **open type** behaves like a *struct*, where you can add data members without changing the definition of that *struct*.

This could be trivially implemented by a map from some kind of tag to an `any`.  
This library offers an efficient implementation with two indirections and type-safe access.

### *Ad hoc* Type Erasure Versus Architectural *Type Tunnel*
**Type erasure**, as we see it today, and how it is supported by well-known libraries, often shares this pattern:

An input parameter of a function can consume any object, as long as it conforms to the syntactic requirements of the **type-erasing** parameter.

This technique solves many problems but stops working as soon as the information passed to the **type eraser** participates in *inversion of control*.

This **virtual_void** library is designed to solve this problem with **type tunneling**. Recovering the *erased* information and casting to different *interfaces* of that information is the key.

[expression problem]: https://en.wikipedia.org/wiki/Expression_problem  
[open multi-methods]: https://en.wikipedia.org/wiki/Multiple_dispatch
