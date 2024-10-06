#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "include/catch.hpp"

#include "../include/virtual_void/m_table/lifetime.h"
#include "../include/virtual_void/m_table/open_method.h"
#include "../include/virtual_void/open_method/algorithm.h"

#include "class_hierarchy_test_hierarchy.h"

namespace
{

using namespace virtual_void::m_table;
using namespace TestDomain;

auto ToString = []( const auto* t )->std::string{ return typeid( *t ).name(); };  

using to_string_method = open_method< std::string( const void* ) >;

template< typename T > auto call( const to_string_method& method )
{ 
	T t;
	return method( virtual_void::to_m_table_void( &t ) );
}


TEST_CASE( "m_table open_method" ) 
{
	{
		domain testDomain;
		to_string_method toString( testDomain );
				
		toString.define< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );

		virtual_void::open_method::declare_classes( virtual_void::type_list< D >{}, testDomain );
		register_m_tables< virtual_void::type_list< D > >( testDomain );
		virtual_void::open_method::interpolate( testDomain );
		REQUIRE( toString.is_defined< D >() );
		fix_m_tables( testDomain );
		REQUIRE( call< D >( toString ) == typeid( A1 ).name() ); // call for base class
	}
	{
		domain testDomain;
		to_string_method toString( testDomain );
				
		virtual_void::open_method::declare_classes( virtual_void::type_list< D >{}, testDomain );
		register_m_tables< virtual_void::type_list< D > >( testDomain );
		virtual_void::open_method::fill_with_overloads< D >( toString, ToString );
		REQUIRE( toString.is_defined< D >() );
		fix_m_tables( testDomain );
		REQUIRE( call< D >( toString ) == typeid( D ).name() ); // call for function found via fill_with_overloads
	}
	{
		domain testDomain;
		to_string_method toString( testDomain );
		using classes = virtual_void::type_list< D, C1, C2 >;
	    virtual_void::open_method::declare_classes( classes{}, testDomain );
		register_m_tables< classes >( testDomain.m_table_map );
		virtual_void::open_method::fill_with_overloads( classes{}, toString, ToString );
		fix_m_tables( testDomain );
		virtual_void::class_hierarchy::visit_classes< classes >( 
			virtual_void::overload
			{ [&]< typename C >				
			{	
				C c;
				auto virtual_void = virtual_void::to_m_table_void( &c );
				auto u = make_unique< C >();
				auto expected = typeid( C ).name();
				auto r = toString( u );
				auto r1 = toString( virtual_void );
				REQUIRE( r == expected );
				REQUIRE( r1 == expected );
			}
			, [&]< typename C, typename B >	{}
			});
	}
}

}
