---
title: terms and definitions
nav_order: 2
---

# terms and definitions in context of this documentation

###  erased interface
An object, which members are function pointers, and every function has as first parameter "self". This is a pointer to (eventually const )void.

###  implemented erased interface
An object derived from an interface, with no additional members and where all members point to vaild functions.
These funtion are filled by a templated constructor. The template parameter is called the erased type.
These functions cast the self parameter to a pointer to the erased type, and delegate via this pointer to the function, for which this was erased through self.

### lifetime holder
An object that manages the lifetime of an other object in a defined way and delivers a void pointer of that object. In our library the member function delivering this pointer is "data()". There are four archetypes:
- observer: Takes no ownership. The Creator of such an observer is responsible for asuring, that the referenced object outlives the observer. There are two flavors: const and mutable, for read only or modifying access to the referenced object.
- shard const: Ownership as std::shard_ptr. The delivered address is a pointer to const.
- unique: Ownership as std::unique_ptr. The delivered address is a pointer to a mutable object.
- value: Every value object holds an own copy. Same semantics as 'int'. The delivered access is modifyable.

### erased versus concrete lifetime holders
An erased lifetime holder holds no compiletime information about the "holded" object. Only if the holder is decoratet with some "virtual meta data", there is a dynamic and safe cast of the void pointer to conrete pointer possible.
A concrete lifetime holdder delivers the a pointer to an object of type, with which the object was constructed. A concrete lifeteime holder can alwayes be converted in an aerased lifetime holder.

### static cast vs dynamic cast
- A static cast
  - is in the language type rules and always checked ba the compiler. 
  - Upcasts are always safe und static. 
  - Static dowcasts are unsafe.
  - Static casts are only a syntactic construct an leave no trace in the binary code.
- A dynamic cast is
  - a runtime query to test if the casted object is of this type
  - if such a query succeeds, is determined by the programm
  - all dynamic casts are safe
  - dynamic cast need code to run

### upcast vs downcast
An upcast is a conversion from a more detailed type to a general one.
A dowcast is conversione from a more general type to a more detailed ond.
For upcast and downcasts must the types be related within the language rules.
Staic downcasts are guesses and as such unsafe. Dynamic downcasts are per definition safe.

### crosscast
While up and downcast are within related types, crosscasts are between unrelated types. Such types are typicaly virtual base types (an interface).  
A crosscast usualy tests, if one interface can be reached from an other, and if so give access to it.

### open method
an open method is a freestanding callable, which 


