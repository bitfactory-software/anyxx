---
title: virtual void
layout: home
nav_order: 1
permalink: /
---

# virtual void - a vocabulary library for programming in the large
# terms and definitions in context of this documentation

##  erased interface
An object, whitch , members are function pointers, and every function has as first parameter "self". This is a pointer to (eventually const )void.

##  implemented erased interface
An object derived from an interface, with no additional members and where all members point to vaild functions.
Thes funtion are filled by a templated consztuctuer. The pemplate parameter is called the erased type.
These functions cast the self parameter to a pointer to the erased type, and delegete via this pointer to the function, for witch this was erased through self.

## lifetime holder
An object with manages the lifetime of an other object in a defined way and deleiver an address of that object. There are four archetypes:
### observer
Takes no ownership. The Creator of such an observer is responsible for asureing, that the referenced object outlives the observer. There are two flavors: const and mutable, for read only or modifying acces to the referenced object.
### shard const
Ownership as std::shard_ptr. The deliverd address is a pointer const.
### unique
Ownership as std::unique_ptr. The deliverd address is a pointer to a mutable object.
### value
Every value object holds an own copy. Same semantis as 'int'. The delivered access is modifyable.

## erased lifetime holders

## static cast

## dynamic cast

## upcast 
static

## downcast
static, dynamic

## crosscast
dynamic , from one "interface" to an other

## open method


[GitHub Pages]: https://docs.github.com/en/pages
[README]: https://github.com/just-the-docs/just-the-docs-template/blob/main/README.md
