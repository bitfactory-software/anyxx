#pragma once

#include <iostream>
#include <typeinfo>

#include "simple_open_method/declare.h"
#include "simple_open_method/interpolate.h"
#include "simple_open_method/fill_with_overloads.h"
#include "class_hierarchy/class_hierarchy.h"

#include "class_hierarchy_test_hierarchy.h"

namespace BitFactory::simple_open_method
{
	namespace
	{
		auto ToString = []( const auto* t )->std::string{ return typeid( t ).name(); };  

		template< typename T > void call( const declare< std::string, const void* >& method )
		{ 
			T t;
			std::cout << method( to_typed_void( &t ) ) << "\n";
		}

		void test_simple_open_method()
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";

			using namespace TestDomain;

			std::cout << "\n";
			std::cout << "dispatch via void" << "\n";
			{
				auto toString = declare< std::string, const void* >{};
				
				toString.define< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
				toString.seal();

				call< A1 >( toString );

				auto tv = to_typed_void( static_cast< A1* >( nullptr ) );
				std::cout << toString( tv ) << "\n";
				try
				{
					call< D >( toString );
					std::cout << "error: should not work!" << "\n";
				}
				catch( simple_open_method::error& error )
				{
					std::cout << error.what() << " as expected." << "\n";
				}
			}
			{
				auto toString = declare< std::string, const void* >{};
				
				toString.define< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
				class_hierarchy::classes_with_bases registry;
				declare_deep< D >( registry );
				interpolate( toString, registry );
				std::cout << "toSring.is_defined< D >() == " << std::boolalpha << (bool)toString.is_defined< D >() << "\n";
				toString.seal();
				call< D >( toString );
			}

			{
				auto toString = declare< std::string, const void* >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( toString, ToString );
				toString.seal();
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ call< C >( toString ); }
					, [&]< typename C, typename B >	{}
					});
			}
		}
	}
}