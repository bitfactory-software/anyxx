#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "include/catch.hpp"

#include "virtual_void/erased/data/has_type_info/observer.h"
#include "virtual_void/erased/open_method/via_type_info/declare.h"
#include "virtual_void/erased/open_method/algorithm.h"

#include "class_hierarchy_test_hierarchy.h"

using namespace ::virtual_void;
using namespace ::virtual_void::erased;
using namespace ::virtual_void::erased::data::has_type_info;

namespace
{

	auto ToString = []( const auto* t )->std::string{ return typeid( *t ).name(); };  

	using to_string_method = open_method::via_type_info::declare< std::string( const void* ) >;

	template< typename T > std::string call( const to_string_method& method )
	{ 
		T t;
		return method( &t );
	}

	TEST_CASE( "typeid open_method" ) 
	{
		using namespace TestDomain;

		{
			open_method::via_type_info::domain open_methods;
			to_string_method toString( open_methods );
				
			toString.define< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
			toString.seal_for_runtime();

			REQUIRE( call< A1 >( toString ) == typeid( A1 ).name() );

			auto tv = static_cast< A1* >( nullptr );
			REQUIRE( toString( tv ) == typeid( A1 ).name() );
			try
			{
				call< D >( toString );
				REQUIRE( false );
			}
			catch( error& )
			{
				REQUIRE( true );
			}
		}

		{
			open_method::via_type_info::domain open_methods;
			to_string_method toString( open_methods );
			using classes = type_list< D, C1, C2 >;
			open_method::fill_with_overloads( classes{}, toString, ToString );
			seal_for_runtime( open_methods );
			class_hierarchy::visit_classes< classes >( 
				overload
				{ [&]< typename C >				{ REQUIRE( call< C >( toString ) == typeid( C ).name() ); }
				, [&]< typename C, typename B >	{}
				});
		}
	}
}
