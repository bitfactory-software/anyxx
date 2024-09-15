#pragma once

#include <iostream>
#include <typeinfo>

#include "simple_open_method.h"
#include "interpoalte_simple_open_method.h"
#include "class_hierarchy.h"

#include "class_hierarchy_test_hierarchy.h"

namespace BitFactory::simple_open_method
{
	namespace
	{

		template< typename T > std::string ToString( T* ){ return typeid( T ).name(); }  

		void test_simple_open_method()
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";

			using namespace TestDomain;

			{
				auto toString = declare< std::string >{};
				
				toString.define< A1 >( []( A1* x ){ return ToString( x ); } );

				std::cout << toString( static_cast< A1* >( nullptr ) ) << "\n";
				try
				{
					std::cout << toString( static_cast< D* >( nullptr ) ) << "\n";
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
				std::cout << toString( static_cast< D* >( nullptr ) ) << "\n";
			}
		}
	}
}