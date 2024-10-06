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

template< typename T > void call( const to_string_method& method )
{ 
	T t;
	std::cout << method( virtual_void::to_m_table_void( &t ) ) << "\n";
}


TEST_CASE( "m_table open_method" ) 
{
	std::cout << "\n";
	{
		domain testDomain;
		to_string_method toString( testDomain );
				
		toString.define< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );

		virtual_void::class_hierarchy::classes_with_bases classes;
		virtual_void::class_hierarchy::declare_deep< D >( classes );
		fix_m_tables( testDomain );
		std::cout << "toSring.is_defined< D >() == " << std::boolalpha << (bool)toString.is_defined< D >() << "\n";
		call< D >( toString );
	}

	//{ /typeid test
	//	domain testDomain;
	//	to_string_method toString( testDomain );
	//	using classes = virtual_void::type_list< D, C1, C2 >;
	//	virtual_void::open_method::fill_with_overloads( classes{}, toString, ToString );
	//	virtual_void::open_method::seal( testDomain );
	//	virtual_void::class_hierarchy::visit_classes< classes >( 
	//		virtual_void::overload
	//		{ [&]< typename C >				{ call< C >( toString ); }
	//		, [&]< typename C, typename B >	{}
	//		});
	//}

	{
		domain testDomain;
		to_string_method toString( testDomain );
		using classes = virtual_void::type_list< D, C1, C2 >;
		virtual_void::open_method::fill_with_overloads( classes{}, toString, ToString );
		register_m_tables< classes >( testDomain.m_table_map );
		fix_m_tables( testDomain );
		virtual_void::class_hierarchy::visit_classes< classes >( 
			virtual_void::overload
			{ [&]< typename C >				
			{	
				C c;
				auto virtual_void = virtual_void::to_m_table_void( &c );
				auto u = make_unique< C >();
				auto expected = typeid( C ).name();
				std::cout << "virtual_void dispatch for " << expected << ": "; 
				auto r = toString( u );
				auto r1 = toString( virtual_void );
				if( r != expected || r1 != expected )
					std::cout << "fail: " << r << ", " << r1;
				else
					std::cout << "OK";
				std::cout << std::endl;
			}
			, [&]< typename C, typename B >	{}
			});
	}
}

}
