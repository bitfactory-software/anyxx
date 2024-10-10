---
title: virtual void
layout: home
nav_order: 1
permalink: /
---

# virtual void - a vocabulary library for programming in the large

You will find for your disposal these **type erased** tools:
- lifetimehandles
- interfaces
  - deriveable
  - upcastable
  - downcastable
- open methods
- object casts
- open objects

These types are orthogonal building blocks for large software systems with loose coupling and performance on par with virtual function calls. 

The library is **header only** 

The name of the library originates in its fundamental design idea:
- hide the concrete data behind a (possible "smart") **pointer to void**,
- pair this with a pointer to **virtual** meta data to interprete the hidden data
- and use such pairs as the external interfaces, to seperate usage and implementation details as much as possible 

# programming in the large: runtime versus compiletime dispatch

Dispatch on behalf of a type is like function calls a fundamental  construction principle, to splitt software in smaler parts.
They allow the building of layerd abstraction levels.
Eceeds a program a certain size, it is no longer possible to solve   
