#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/virtual_void/m_table/lifetime.h"

#include "class_hierarchy_test_hierarchy.h"


using namespace Catch::Matchers;

using namespace virtual_void::m_table;
using namespace TestDomain;

namespace
{
    TEST_CASE( "m_table/lifetime/shared_const" )
    {
		auto d = make_shared_const< D >( "shared hallo" );
		shared_const x = as< D >( d );
		auto d1 = as< D >( x );
		std::cout << d->data << ", " << x.type().name() << std::endl;
		REQUIRE( d->data == "shared hallo" );
		REQUIRE( d.type() == typeid( D ) );
		static_assert( std::derived_from< D, A1 > );
		typed_shared_const< A1 > a1 = d1;
		typed_shared_const< A1 > a2 = A1{ "a2->OK" };
		typed_shared_const< A1 > a3{ std::in_place, "a3 in_place->OK" };
		A1 a1_pur{ "a1_pur" };
		typed_shared_const< A1 > a4{ a1_pur };
		auto& a1r = *a2;
		auto s1 = a1r.data;
		auto s = a2->data;
		std::cout << a2->data << ", " << a2.type().name() << std::endl;				
		std::cout << a3->data << ", " << a3.type().name() << std::endl;
		std::cout << a4->data << ", " << a4.type().name() << std::endl;
		REQUIRE( a2->data == "a2->OK" );
		REQUIRE( a3->data == "a3 in_place->OK" );
		REQUIRE( a4->data == "a1_pur" );
	}

    TEST_CASE( "m_table/lifetime/unique" )
    {
		auto c1 = make_unique< C >( "unique c1"); 
		std::cout << c1->data << ", " << c1.type().name() << std::endl;				
		REQUIRE( c1->data == "unique c1" );
		auto c2 = typed_unique< C >( std::in_place, "unique c2" ); 
		std::cout << c2->data << ", " << c2.type().name() << std::endl;				
		REQUIRE( c2->data == "unique c2" );
		auto c3 = typed_unique< C >( C{ "unique c3" } ); 
		REQUIRE( c3->data == "unique c3" );
		std::cout << c3->data << ", " << c3.type().name() << std::endl;				
		auto c4 = std::move( c3 ); 
		std::cout << c4->data << ", " << c4.type().name() << std::endl;				
		REQUIRE( c4->data == "unique c3" );

		auto d1 = make_unique< D >( "unique hallo" );
		unique x{ std::move( d1 ) };
		auto d = as< D >( std::move( x ) );
		std::cout << d->data << ", " << d.type().name() << std::endl; 
		REQUIRE( d->data == "unique hallo" );
	}

}
