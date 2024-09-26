#include <string>
#include <vector>

#include "include/catch.hpp"

#include "../include/perfect_typeid_hash/index_table.h"


TEST_CASE( "build perfect typeid" ) {

	using entry_t = std::pair< perfect_typeid_hash::type_id, const char* >;
	using table_t = std::vector<entry_t>;
	table_t elements ={ { &typeid(int), "int" }, { &typeid(std::string), "std::string" }, { &typeid(entry_t), "entry_t" } }; 

	perfect_typeid_hash::index_table< const char* > index_table( elements );

	REQUIRE_THROWS( index_table[ &typeid(float) ] );

	for( auto element : elements )
		REQUIRE( index_table[ element.first ] == element.second );
}

