---
title: Any like type Tunneling evolution with nearly perfect hash.
parent: blog
nav_order: 30
---

# into the open method range

In our last experiment, we put std::any with a simple typeid dispath in a race against v-tables. 
In this installment we will power up this dispatch and battle once again!

We stumbled into this adventure, because we saw a chance, to escape the vistor pattern hell. 
The invention is not new.
We are now in the realm of "open methods". This are in regard to c++ was investigated by giants before us. See for example [Bjarne Stroustrup](https://www.stroustrup.com/multimethods.pdf) and 
[Jean-Louis Leroy](https://github.com/jll63/yomm2) [(video)](https://www.youtube.com/watch?v=xkxo0lah51s).

This was all so promisssing, but the proposal for the language from Bjarne Stroustrup got not into the standard.
The performce results of Jean-Louis Leroys yomm2 are astonishing. The strong orientation to the simulation of the proposed language extension and along v-tables prevented us von adopting it.

Both ideas develop from the multi-dispatch perspective, witch for our scenarios not realy relevent.
We see the main attraction in the solving of the [expression problem](https://en.wikipedia.org/wiki/Expression_problem) by this aproach.

By studiing the details of yomm2, we found deep inside a perl: A superfast perfect hash function for int64 keys. 

We used the permissive licence to steal this algorithim and to spent him a convinient interface.
You can find the source [here]:
