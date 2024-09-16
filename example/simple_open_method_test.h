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

		template< typename T > std::string ToString( T* ){ return typeid( T ).name(); }  

		template< typename T > void call( const declare< std::string, void >& method )
		{ 
				std::cout << method( static_cast< T* >( nullptr ) ) << "\n";
		}

		void test_simple_open_method()
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";

			using namespace TestDomain;

			{
				auto toString = declare< std::string, void >{};
				
				toString.define< A1 >( []( A1* x ){ return ToString( x ); } );

				call< A1 >( toString );

				std::cout << toString( static_cast< A1* >( nullptr ) ) << "\n";
				try
				{
					call< D >( toString );
					std::cout << "error: should not work!" << "\n";
				}
				catch( simple_open_method::error& error )
				{
					std::cout << error.what() << " as expected." << "\n";
				}
				class_hierarchy::classes_with_bases registry;
				declare_deep< D >( registry );
				interpolate( toString, registry );
				std::cout << "toSring.is_defined< D >() == " << std::boolalpha << (bool)toString.is_defined< D >() << "\n";
				call< D >( toString );
			}

			{
				auto toString = declare< std::string, void >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( toString, []( auto s ){ return ToString( s ); } );
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ call< C >( toString ); }
					, [&]< typename C, typename B >	{}
					});

			}
		}
	}
}