---
title: virtual void
layout: home
nav_order: 1
permalink: /
---

# virtual void

## a library with **type erased** vocabulary types for programming in the large

- lifetimehandles
- interfaces
  - deriveable
  - upcastable
  - downcastable
- open methods
- object casts
- open types

These types are orthogonal building blocks for large software systems with loose coupling and performance on par with virtual function calls. 

The library is **header only** 

The name of the library originates in its fundamental design idea:
- hide the concrete data behind a (possible "smart") **pointer to void**,
- pair this with a pointer to **virtual** meta data to interprete the hidden data
- and use such pairs as the external interfaces, to seperate usage and implementation details as much as possible 

## programming in the large

### runtime versus compiletime dispatch

Dispatch on behalf of a type is like function calls a fundamental  construction principle, to splitt software in smaler parts.
They allow the building of layerd abstraction levels.
Static dispatch is the dispatch of chioce in C++. Foremost because of performance and binary size.
But eceeds a program a certain size, it is no longer possible to use static disapch. Reasons are:
- you do not know all your types all the time
- you can not use all your types all the time because of restricted build resources: time and space.
For example, imagine a std::variant for some hundred different alternatives.
This is where **programming in the large** begins and runtime dispatch is  proven means.

### "OO style" versus "type erasure" runtime dispatch

C++'s virtual functions have shown their versaitilty and usefullness for decades.
But, as the understanding for decompising software grow, we saw that the "OO style" integration of the interface direct into the object as an ever growing problem.
It is now consens, that separation of the interface from the underlying object is the way to go. The type gets erased from the interface.
On the contrary, by doing so, we lose this connection, whitch had shown is usefullnes in a countless homongos and succesful software projects.
So can a v-table interface of an object to be used, to cast back to that object itself, to "downcast" the interface to a more specific one, and to "crosscast" to an other interface.
We see the this casts as an absolute must, type erasure has do deliver, to use it as the fundamnental building block for  programming in the large.
Because we found no library, which fullfilled this demands, we came up with this one.  







