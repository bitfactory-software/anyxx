---
title: perfect typeid hash in detail
parent: blog
nav_order: 40
---

# into the open method range

In our last experiment, we put std::any with a simple typeid dispath in a race against v-tables. 
In this installment we will power up this dispatch and battle once again!

We stumbled into this adventure, because we saw a chance, to escape the vistor pattern hell.
