#pragma once

#include <iostream>

#include "include/catch.hpp"

#include "class_hierarchy_test_hierarchy.h"

namespace virtual_void::class_hierarchy
{
	namespace
	{
		void dump( const classes_with_bases& registry )
		{
			for( const auto& [ _, class_with_bases ] : registry )
			{
				std::cout << class_with_bases.self->name() << " : ";
				for( auto base : class_with_bases.bases )
					std::cout << base->name() << " ";
				std::cout << "\n";
			}
		}
	
		void test_class_hierarchy()
		{
			using namespace TestDomain;

			std::cout << "\n";
			std::cout << "class_hierarchy::test" << "\n";

			{
				classes_with_bases registry;
				declare_shallow< A1 >( registry );
				declare_shallow< A2 >( registry );
				declare_shallow< B1 >( registry );
				declare_shallow< B2 >( registry );
				declare_shallow< B3 >( registry );
				declare_shallow< C1 >( registry );
				declare_shallow< C2 >( registry );
				declare_shallow< C3 >( registry );
				declare_shallow< C >( registry );
				declare_shallow< D >( registry );

				std::cout << "declare_shallow" << "\n";
				dump( registry );
			}

			{
				classes_with_bases registry;
				declare_deep< D >( registry );
				std::cout << "declare_deep< D >" << "\n";
				dump( registry );
			}

			{
				classes_with_bases registry;
				declare_classes< type_list< D, C1, C2 > >( registry );
				std::cout << "declare_deep< D >" << "\n";
				dump( registry );
			}
			{
				classes_with_bases registry;
				declare_classes< type_list< D > >( registry );
				declare_classes< type_list< C1, C2 > >( registry );
				std::cout << "declare_deep< D >" << "\n";
				std::cout << "declare_deep< C1, C2 >" << "\n";
				dump( registry );
			}
		}
	}
}