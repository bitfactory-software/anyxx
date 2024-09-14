#pragma once

#include <iostream>
#include <typeinfo>

#include "simple_open_method.h"
#include "class_hierarchy.h"

namespace BitFactory::simple_open_method
{
	namespace
	{
		struct A1 {};
		struct A2 {};
		struct B1 : A1 {};
		struct B2 : A2 {};
		struct B3 : A1, A2 {};
		struct C1 : B1 {};
		struct C2 : B2 {};
		struct C3 : B3 {};
		struct C {};
		struct D : C, C3 {};

		template< typename T > std::string ToString( T* ){ return typeid( T ).name(); }  


		void test()
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";
			{
				auto toString = declare< std::string >{};
				{
					toString.define< A1 >( []( A1* x ){ return ToString( x ); } );
				}
				std::cout << toString( static_cast< A1* >( nullptr ) ) << "\n";
				//std::cout << toString( static_cast< D* >( nullptr ) ) << "\n";
			}
		}
	}
}