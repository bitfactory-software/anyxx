---
title: Any like type Tunneling evolution with nearly perfect hash.
parent: blog
nav_order: 30
---

# into the open method range

In our last experiment, we put std::any with a simple typeid dispath in a race against v-tables. 
In this installment we will power up this dispatch and battle once again!

We stumbled into this adventure, because we saw a chance, to escape the vistor pattern hell and can pass information through the system with a type tunnel.

The idea of a freestanding dispath via some type information is not new. (It is in fact realy old).
We are now in the realm of "open methods". This are in regard to c++ was investigated by giants before us. See for example [Bjarne Stroustrup](https://www.stroustrup.com/multimethods.pdf) and 
[Jean-Louis Leroy](https://github.com/jll63/yomm2) [(video)](https://www.youtube.com/watch?v=xkxo0lah51s).

This was all so promisssing, but the proposal for the language from Bjarne Stroustrup got not into the standard.
The performce results of Jean-Louis Leroys yomm2 are astonishing. The strong orientation to the simulation of the proposed language extension and along v-tables prevented us von adopting it.

Both ideas develop from the multi-dispatch perspective, witch for our scenarios not realy relevent.
We see the main attraction in the solving of the [expression problem](https://en.wikipedia.org/wiki/Expression_problem) by this aproach.

By studiing the details of yomm2, we found deep inside a perl: A superfast perfect hash function for int64 keys. 

We used the permissive licence to steal this algorithim and to spent him a convinient interface.
You can find the source [here](https://github.com/andreaspfaffenbichler/virtual_void/blob/master/include/virtual_void/perfect_typeid_hash/index_table.h)

Let's look at the usage of the algorithm:
```
#include <string>
#include <vector>

#include <catch.hpp>

#include "virtual_void/perfect_typeid_hash/index_table.h"


TEST_CASE( "build perfect typeid hash index" ) {

	using entry_t = std::pair< perfect_typeid_hash::type_id, const char* >;
	using table_t = std::vector<entry_t>;
	table_t elements ={ { &typeid(int), "int" }, { &typeid(std::string), "std::string" }, { &typeid(entry_t), "entry_t" } }; 

	auto not_found = "not found";
	perfect_typeid_hash::index_table< const char* > index_table( elements, not_found );

	REQUIRE( index_table[ &typeid(float) ] == not_found );

	for( auto element : elements )
		REQUIRE( index_table[ element.first ] == element.second );
}
```

The first  three lines desribe the values, you want a perfect hash for.
it must be a range of pairs. The first is the address of a std::type_info, and the second anything with an bool operator.
These are the "elements".

The constructor of the "perfect_typeid_hash::index_table" takes the "elemnents" and an error value. This error value is of the same type as the second in the elemnets vector.
Inside the constructor happens the magic: The parameter for the fast hash function are computed. 
Once constructed, the index_table is not mutable.

The searched value is accessd with the [] operator, where the "index" is a pointer to typeid. When the index is not in the elemnets, the "error value" is returned.

For the interessted on the inner workings, we will do an extra short blog, to go through the details.

But now let us prepare for our next round in the runtime dispatch battle.

We install the perfect_typeid_hash::index_table into the [any_dispatch method](https://github.com/andreaspfaffenbichler/virtual_void/blob/master/include/virtual_void/any_dispatch/method_typeid_hash.h).

New is also the "ri·en ne va plus" member function "seal". It constructs the index_table and drops the map used to collect the elements.

Now we are back in the [expression tree arena with the ramped up any dispatch](https://github.com/andreaspfaffenbichler/virtual_void/blob/master/test/21_Tree_TE_dispatch_via_any_and_type_index.cpp):
We need only to change to the "any_dispatch::method_typeid_hash" and "seal" the three functions for "value", "to_forth" and "to_lisp".

And we are at the results:
```
-------------------------------------------------------------------------------
20_Tree_OO
-------------------------------------------------------------------------------
D:\BitFactory\Blog\virtual_void\test\20_Tree_OO.cpp(63)
...............................................................................

benchmark name                       samples       iterations    est run time
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
20_Tree_OO value                               100          3067     1.2268 ms
                                        4.41865 ns    4.40561 ns    4.43984 ns
                                      0.0829191 ns  0.0473516 ns   0.142189 ns

20_Tree_OO as_lisp                             100           152     1.4288 ms
                                        89.1316 ns    88.0197 ns     90.875 ns
                                        7.00705 ns    5.24271 ns    10.3727 ns


2 3 4 + * = (times 2 (plus 3 4)) = 14
-------------------------------------------------------------------------------
21_Tree_TE_dispach_via_any_dispatch
-------------------------------------------------------------------------------
D:\BitFactory\Blog\virtual_void\test\21_Tree_TE_dispatch_via_any.cpp(88)
...............................................................................

benchmark name                       samples       iterations    est run time
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
21_Tree_TE_dispach_via_any_dispatch
value                                          100           239     1.4101 ms
                                        56.3431 ns    55.6192 ns    57.3305 ns
                                        4.24858 ns    3.35959 ns     5.2938 ns

21_Tree_TE_dispach_via_any_dispatch
as_lisp                                        100           111     1.4208 ms
                                        153.523 ns    139.351 ns    213.486 ns
                                        127.894 ns    22.8102 ns    300.292 ns


2 3 4 + * = (times 2 (plus 3 4)) = 14
-------------------------------------------------------------------------------
21_Tree_TE_dispatch_via_any_and_type_index
-------------------------------------------------------------------------------
D:\BitFactory\Blog\virtual_void\test\21_Tree_TE_dispatch_via_any_and_type_index.cpp(88)
...............................................................................

benchmark name                       samples       iterations    est run time
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
21_Tree_TE_dispatch_via_any_and_ty-
pe_index value                                 100           414     1.4076 ms
                                        29.6014 ns    29.4396 ns    30.0483 ns
                                        1.26386 ns   0.550619 ns    2.69191 ns

21_Tree_TE_dispatch_via_any_and_ty-
pe_index as_lisp                               100           125      1.425 ms
                                        101.832 ns    100.392 ns    104.104 ns
                                        9.02631 ns    6.68371 ns    13.9526 ns


2 3 4 + * = (times 2 (plus 3 4)) = 14
```

That is satisfying. The "value" case is nearly 50% down and for "as_lisp" the overhead is now only 10% compared to v-table dispatch.
Things starts to get practically usefull.

If you are interesstet in the details, you can read more here in this "special blog post".

For now we will summarize the remaining bottlenecks with our any_dispatch:

- "std::function" gives us an extra indirection for the type erasure
- "std::any_cast" checks, if we access with right typeid. In our case is this redundant: we are here because we were found for this typeid.

Next time, we will replace any and function to see, how far we get with this.











