#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "include/catch.hpp"

#include "../include/virtual_void/typeid/open_method.h"
#include "../include/virtual_void/open_method/algorithm.h"

#include "class_hierarchy_test_hierarchy.h"

namespace
{
	using namespace virtual_void::typeid_;

	auto ToString = []( const auto* t )->std::string{ return typeid( *t ).name(); };  

	using to_string_method = open_method< std::string( const void* ) >;

	template< typename T > std::string call( const to_string_method& method )
	{ 
		T t;
		return method( &t );
	}

	TEST_CASE( "typeid open_method" ) 
	{
		using namespace TestDomain;

		std::cout << "\n";
		std::cout << "dispatch via void" << "\n";
		{
			domain open_methods;
			to_string_method toString( open_methods );
				
			toString.define< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
			toString.seal_for_runtime();

			REQUIRE( call< A1 >( toString ) == typeid( A1 ).name() );

			auto tv = virtual_void::to_typeid_void( static_cast< A1* >( nullptr ) );
			std::cout << toString( tv ) << "\n";
			try
			{
				call< D >( toString );
				REQUIRE( false );
			}
			catch( virtual_void::error& )
			{
				REQUIRE( true );
			}
		}

		{
			domain open_methods;
			to_string_method toString( open_methods );
			using classes = virtual_void::type_list< D, C1, C2 >;
			virtual_void::open_method::fill_with_overloads( classes{}, toString, ToString );
			seal_for_runtime( open_methods );
			virtual_void::class_hierarchy::visit_classes< classes >( 
				virtual_void::overload
				{ [&]< typename C >				{ REQUIRE( call< C >( toString ) == typeid( C ).name() ); }
				, [&]< typename C, typename B >	{}
				});
		}
	}
}
