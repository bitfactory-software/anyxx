---
title: perfect typeid hash in detail
parent: blog
nav_order: 40
---

# Jean-Louis Leroy's "perfect typeid hash"

For his [yomm2](https://github.com/jll63/yomm2) library Jean-Louis Leroy developed an fast algorithm to **find from the addres of a std::type_info to a pointer**.
Because he published it under the permissive BOOST we could use the algorithm for this library and refactor it for our purposes.

We will provide a short walk throug  it:

The goal of the hash index is, to compute as fast as possible a hash value from the ***std::type_info* ***, that can be directly used as an index in an array (std::vector) containig the searched target, where all possible values for the ***std::type_info* *** (=type_id from now on) are known.

This function shall be a multiplcation with ***mult*** and a right shift with ***shift***:
```
    index_t apply_formula(type_id type) const {
      return (reinterpret_cast<std::size_t>(type) * mult) >> shift;
    }
```
So the art is, to find ***perfect*** values for ***mult*** and ***shift***.

| size    | inital_sparse_factor |
| ----: | -------------------: |
| 5       | 4   |          
| 25      | 6   |
| 625     | 11  |




