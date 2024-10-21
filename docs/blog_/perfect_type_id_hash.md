---
title: perfect typeid hash in detail
parent: blog
nav_order: 40
---

# Jean-Louis Leroy's "perfect typeid hash"

For his [yomm2](https://github.com/jll63/yomm2) library Jean-Louis Leroy developed an fast algorithm to **find from the addres of a std::type_info to a pointer**.
Because he published it under the permissive BOOST license, we could use the algorithm for this library and refactor it for our purposes.

We will provide here a short walk through:

The goal of the hash index is, to compute as fast as possible a hash value from the *std::type_info* *, that can be directly used as an index in an array (std::vector) containig the searched target, where all possible values for the *std::type_info* * (=*type_id* from now on) are known.

This function shall be a multiplcation with *mult* and a right shift with *shift*:
```
    index_t apply_formula(type_id type) const {
      return (reinterpret_cast<std::size_t>(type) * mult) >> shift;
    }
```
So the art is, to find *perfect* values for *mult* and *shift* in a given range of *elements*.
Each *elenent* is a pair of type_id and the target, witch we wount to fast as possible for thah  type_id. 
To make this a task, that can end before the next big bang, the table containig the targets neads spare space.
The algorithm starts with litle spare and tries to find values for *mult* and *shift*, so that the result of *apply_formula(type_id)* is unique for every type_id.
If this fails, the spare space is increased, and the search for *mult* and *shift* is repeated.

This table shows the initial *spare_base* value for some sizes of *elements*:
```
  auto static inital_sparse_base(std::size_t element_count) {
    std::size_t sparse_base = 1;
    for (auto size = element_count * 5 / 4; size >>= 1;) ++sparse_base;
    return sparse_base;
  }
```
| size    | inital_sparse_base | 
|--------:|-------------------:|
| 10    | 4                    |
| 100 | 7 |
| 1000 | 11 |
| 10000 | 14 |
| 100000 | 17 |
| 1000000 | 21 |

This table shows, how the spare_base relates to the table size. The values should be familar
```
  static auto size_for_sparse_base(std::size_t sparse_base) {
    return std::size_t(1) << sparse_base;
  }
```
| sparse_base | table.size    |
|------------:|--------------:|
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

For a given *sparse_base* the *shift* is set to
```
    hash_index.shift = 8 * sizeof(type_id) - sparse_base;
```
Then the the algorithms trys to find via an random number generator a *mult* that maps each *type_id* to its own index.
```
static std::optional<hash_index> find_hash_for_sparse_base(
    const auto& elements, std::size_t sparse_base) {
  hash_index hash_index;
  hash_index.shift = 8 * sizeof(type_id) - sparse_base;
  hash_index.table.resize(size_for_sparse_base(sparse_base));
  hash_index.length = 0;

  std::default_random_engine random_engine(13081963);
  std::uniform_int_distribution<std::uintptr_t> uniform_dist;

  for (std::size_t attempts = 0; attempts < 100000; ++attempts)
    if (can_hash_input_values(elements, hash_index,
                              (uniform_dist(random_engine) | 1)))
      return hash_index;

  return {};
}
``` 
The spare factor is increasd, if this fails 100000 times.  


