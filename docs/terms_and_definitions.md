---
title: terms and definitions
nav_order: 2
---

# terms and definitions in context of this library

###  erased interface
An object, which members are function pointers, and every function has as **first parameter *self***. This is a pointer to (eventually *const*) *void*.

###  implemented erased interface
#### An object derived from an interface, with no additional members and where all members point to vaild functions.

These funtion are filled by a templated constructor. The template parameter is called the **unerased** type.
In this functions will "self" parameter be casted back to a pointer to the unerased type, and correct function for the "unerased" type wil be called. The default for the called function is a member function with the same name and signature as specified for the interface function. This behaviour can be cutomized in an **interface map** for the unerased type.

### virtual_void

#### An object that **erases** the real **type** und the **lifetime** of **an** other **object**. It delivers a *void* pointer of that object. In our library the member function delivering this pointer is "data()". There are four archetypes:
- observer: Takes no ownership. The Creator of such an observer is responsible for asuring, that the referenced object outlives the observer. There are two flavors: *const* and *mutable*, for read only or modifying access to the referenced object.
- *shard_const*: Ownership as std::shard_ptr. The delivered address is a pointer to *const void*
- *unique*: Ownership as std::unique_ptr. The delivered address is *void* a pointer to a *mutable* object.
- *value*: Every value object holds an own copy. Same semantics as *int*. The delivered *void* pointer is *mutable*.
An *virtual_void* object referncees an *meta* object.

#### There are three kinds of meta objects in the library:
- *has_no_meta*
- *has_type_info*
- *has_m_table* has *type_info* and a pointer to a *m_table* for fast dispatch in an *open method*

### virtual_void versus typed_void 
#### A *virtual_void* holds no compiletime information about the *holded* object. 

Only if *virtual_void* has *runtime meta data", there is a dynamic and safe cast of the *void* pointer to conrete pointer possible.
virtual_void has two kinds of such meta data:
- typeid
- m_table
A  *virtual_typed* is a typed wrapper atop of *virtual void*. If the *virtual void* holds in its meta an *type info*, the cast to *virtual_void* is a safe cast.

### static cast vs dynamic cast
#### A *static cast*
  - is in the language type rules and always checked ba the compiler. 
  - Upcasts are always safe und static. 
  - Static dowcasts are unsafe.
  - Static casts are only a syntactic construct an leave no trace in the binary code.
#### A *dynamic cast* is
  - a runtime query to test if the casted object is of this type
  - if such a query succeeds, is determined by the programm
  - all dynamic casts are safe
  - dynamic cast need code to run

### upcast vs downcast
#### An *upcast* is a *conversion* from a more detailed type to a general one.

A dowcast is conversione from a more general type to a more detailed ond.
For upcast and downcasts must the types be related within the language rules.
Staic downcasts are guesses and as such unsafe. Dynamic downcasts are per definition safe.

### lifetime cast
#### A *virtual_void* can be casted to an other *virtual_void*, in the sense of an *upcast*.

- a "smart" lifteime holder ("shared", "uniqe", "value") can be casted to an "observer"
- a mutable liftime holder can be casted to an read only "observer".
But not in the other direction.

### crosscast
#### While *up-* and *downcasts* are within related types, *crosscasts* are between unrelated types. 

Such types are typicaly virtual base types (an interface).  
A *crosscas*t usualy tests, if one interface can be reached from an other, and if so give access to it.

### open method
#### An **open method** is a freestanding callable, which acts like a virtual member function. 

Open member functions are an reciepe to solve the [expression problem]. An opem method is the simpliest, but very usefull, case of [open multi methods]. With an open method you can add a function, whitch behaviour is determined by the type of its (first) arguent, but you do not need to change the definition of that type. 

### open type
#### An **open type** is behaves like a *struct*, where you can add data members without changing the definition of that *struct*. 

This could be trivially implemented by an map from some kind of tag to an any.
This library offers an efficient implentation with two indirections and typesafe access.

### *ad hoc* type erasure versus architectural *type tunnel*
**type erasure** as we see it today, and how it is supported by the well known libraries, share often this pattern:
An input parameter of a function can consume any object, as long as it conforms to the syntactic requirements from the has an **type erasing** parameter.
This technique solves many problems, but stops working, as soon as the information passed to the **type eraser** participates in *inversion of control*.
This **virtual_void** library is designed to solve this problem with **type tunneling**. Recovering the *erased* information, and casting to different *interfaces* of that information is here the key.  








[expression problem]: https://en.wikipedia.org/wiki/Expression_problem
[open multi methods]: https://en.wikipedia.org/wiki/Multiple_dispatch
