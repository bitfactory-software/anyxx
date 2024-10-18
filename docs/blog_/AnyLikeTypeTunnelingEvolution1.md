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
You can find the source [here](https://github.com/andreaspfaffenbichler/virtual_void/blob/master/include/virtual_void/perfect_typeid_hash/index_table.h)

Before we go into the details, let's look at the usage of the algorithm:
```
#include <string>
#include <vector>

#include "include/catch.hpp"

#include "../include/virtual_void/perfect_typeid_hash/index_table.h"


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
it must be a std::vector of pairs. The first is the address of a std::type_info, and the second anything with an bool operator.
These are the "elements".

The constructor of the "perfect_typeid_hash::index_table" takes the "elemnents" and an error value. This error value is of the same type as the second in the elemnets vector.
Inside the constructor happens the magic: The parameter for the fast hash function are computed. 
Once constructed, the index_table is not mutable.

The searched value is accessd with the [] operator, where the "index" is a pointer to typeid. When the index is not in the elemnets, the "error value" is returned.








