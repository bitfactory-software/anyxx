---
title: "Any like type Tunneling" evolution with (nearly) perfect hash.
parent: blog
nav_order: 20
---

# into the open method range

In our last experiment, we put std::any with a simple typeid dispath in a race against v-tables. 
In this installment we will power up this dispatch and battle once again!

We stumbled into this adventure, because we saw a chance, to escape the vistor pattern hell.
