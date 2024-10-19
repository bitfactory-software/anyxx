---
title: perfect typeid hash in detail
parent: blog
nav_order: 40
---

# Jean-Louis Leroy's "perfect typeid hash"

For his [yomm2](https://github.com/jll63/yomm2) library Jean-Louis Leroy developed an fast algorithm to find from the addres of a std::type_info to a pointer.


In our last experiment, we put std::any with a simple typeid dispath in a race against v-tables. 
In this installment we will power up this dispatch and battle once again!

We stumbled into this adventure, because we saw a chance, to escape the vistor pattern hell.
