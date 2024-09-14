#pragma once

#include <iostream>

#include "class_hierarchy.h"

namespace BitFactory::class_hierarchy
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

		void test()
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";
			BitFactory::classes_with_bases registry;
			DeclareBases< A1 >( registry );
			DeclareBases< A2 >( registry );
			DeclareBases< B1, A1 >( registry );
			DeclareBases< B2, A2 >( registry );
			DeclareBases< B3, A1, A2 >( registry );
			DeclareBases< C1, B1 >( registry );
			DeclareBases< C2, B2 >( registry );
			DeclareBases< C3, B3 >( registry );
			DeclareBases< C >( registry );
			DeclareBases< D, C, C3 >( registry );

			for( const auto& [ _, class_with_bases ] : registry )
			{
				std::cout << class_with_bases.self->name() << " : ";
				for( auto base : class_with_bases.bases )
					std::cout << base->name() << " ";
				std::cout << "\n";
			}
		}
	}
}