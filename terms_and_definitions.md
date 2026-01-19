# Terms and Definitions in Context of This Library

### v_table
An object whose members are function pointers, and every function has as **its first parameter *self***. This is a pointer to (eventually *const*) *void*.
The *v_table* points to a "meta_data* object and optional to a *m_table*.

### v_table instance
#### An object derived from v_table, with no additional members, and where all members point to valid functions.

These functions are implemented via a templated constructor. The template parameter is called the **unerased** type.  
In these functions, the "self" parameter is cast back to a pointer to the unerased type, and the correct function for the "unerased" type will be called. The default for the called function is a member function with the same name and signature fitting to v_table function. This behavior can be customized in a **concept_map** for the unerased type. [Tutorial](/tutorials/tutorial__30.md)

### dispatch_table, dispatch_table instance, multi dispatch matrix 
Similar to v_table, but if an entry is in ivolved in a **multi dispatch**, it describes the dispatch index in the **multi dispatch matrix**. 
The target function for the *multi diapatch* resides in the **multi dispatch matrix**.

### erased_data
#### A concept describing an object that **erases** the **type** of **another** object for a given *lifetime* kind.

The description of these types is implemented via a specialication of *erased_data_trait*.  

The library offers these *lifetime* holders:
- **observer**: Takes no ownership. The creator of such an observer is responsible for ensuring that the referenced object outlives the observer. There are two flavors: *const* and *mutable*, for read-only or modifying access to the referenced object.
- **shared_const**: Ownership as `std::shared_ptr`. The delivered address is a pointer to *const void*.
- **weak**: Ownership as `std::weak_ptr`. No delivered adress. Use `lock` to gain a shared_const, if the object still exists.
- **unique**: Ownership as `std::unique_ptr`. The delivered address is a pointer to a *mutable* object.
- **value**: Every value object holds its own copy. Same semantics as *int*. The delivered *void* pointer is *mutable*. [Tutorial](/tutorials/tutorial___1.md/#t1)

### 'any' Versus 'typed_any'
#### An *any* combines a **v_table** with an **erased_data**.

#### A *typed_any* is a convenience wrapper of an **any** for the **eunerased** type. 

### Static Cast vs Dynamic Cast
#### A *static cast*:
  - Is in the language type rules and always checked by the compiler.
  - Upcasts are always safe and static.
  - Static downcasts are unsafe.
  - Static casts are only a syntactic construct and leave no trace in the binary code.

#### A *dynamic cast*:
  - Uses meta data to find the queried type.
  - If such a query succeeds, is determined by the program.
  - All dynamic casts are safe.
  - Dynamic casts have a runtime cost.

### 'Upcast' vs 'Downcast'
#### An *upcast* is a *conversion* from a more detailed type to a general one.

#### A *downcast* is a conversion from a more general type to a more detailed one.

For upcasts and downcasts, the types must be related within the language rules.  
Static downcasts are guesses and as such unsafe. Dynamic downcasts are by definition safe and a kind of *type query*.

### Lifetime Cast
#### An **erased_data** can recieve the content of an other **erased_data** with different **liftime** within certain rules.
#### Three ways, three rules:
- *borrow*: assignemnt does not change ownership and constness.
- *move*: left hand can hold ownership, right hand has ownership, left hand const or right hand mutable .
- *clone*: left hand can hold ownership.

### Crosscast
#### While *up-* and *downcasts* are within related types, *crosscasts* are between unrelated types.

A **crosscast** usually tests if one **any** can be reached from another **any**, and if so, provides access to it.
To enable a **crosscast** to a certain **any** for an object, the target *any*s **v_table instance** must be registered for objects class. 
This is done With the *ANY_MODEL...* macros.

### Extension Method 
#### An **extension method** is a freestanding callable, witch acts a dispatch_table_function on the first paramater. (must be an **any**).
#### A **extension multi method** is a generalistaion of the **extension method**, to solve the [multiple dispatch](https://en.wikipedia.org/wiki/Multiple_dispatch) .
**Extension methods** are a recipe to solve the [expression problem]. 

### Extension Members
#### An **extension members** behaves like a *struct*, where you can add data members without changing the definition of that *struct*.

This could be trivially implemented by a map from some kind of tag to an `std::any`.  
This library offers an efficient implementation with index based indirections and type-safe access.

### *Ad hoc* Type Erasure Versus Architectural *Type Tunnel*
**Type erasure**, as we see it today, and how it is supported by well-known libraries, often shares this pattern:

An input parameter of a function can consume any object, as long as it conforms to the syntactic requirements of the **type-erasing** parameter.

This technique solves many problems but stops working as soon as the information passed to the **type eraser** participates in *inversion of control*.

This library is designed to solve this problem with **type tunneling**. Recovering the *erased* information and the abilty to cast to different *any*s is the key.

[expression problem]: https://en.wikipedia.org/wiki/Expression_problem  
[open multi-methods]: https://en.wikipedia.org/wiki/Multiple_dispatch
