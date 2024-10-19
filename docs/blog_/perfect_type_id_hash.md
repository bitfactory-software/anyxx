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

| size    | inital_sparse_base | 
| ----: | -------------------: |
| 10 | 4 |
| 100 | 7 |
| 1000 | 11 |
| 10000 | 14 |
| 100000 | 17 |
| 1000000 | 21 |

| sparse_base | table.size |
| ----: | -------------: |
 | 4 | 16 |
 | 5 | 32 |
 | 6 | 64 |
 | 7 | 128 |
 | 8 | 256 |
 | 9 | 512 |
 | 10 | 1024 |
 | 11 | 2048 |
 | 12 | 4096 |
 | 13 | 8192 |
 | 14 | 16384 |
 | 15 | 32768 |
 | 16 | 65536 |
 | 17 | 131072 |
 | 18 | 262144 |
 | 19 | 524288 |
 | 20 | 1048576 |


