---
title: virtual void
layout: home
nav_order: 1
permalink: /
---

# virtual void - a vocabulary library for programming in the large

You will find types to use as type **erased**
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
- hide the concrete data behind a (possible "smart") pointer to void
- pair this with a pointer to static meta data to interprete the hidden data.



