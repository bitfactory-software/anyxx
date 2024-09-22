#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "../include/virtual_void/virtual_void.h"

#include "class_hierarchy_test_hierarchy.h"

namespace
{
	template< typename CLASSES >
	void run_cast_test( const auto& castMethod, auto make_dispatch_var )
	{
		using namespace TestDomain;
		
		virtual_void::class_hierarchy::visit_classes< CLASSES >( 
			virtual_void::overload
			{ [&]< typename TOP >				
			{	
				const TOP top;
				auto c_typed_void = make_dispatch_var( &top );

				virtual_void::class_hierarchy::visit_class< TOP >
					( virtual_void::overload
					{ [&]< typename X >				
						{ 
							std::cout << "typeid_const_cast: " << typeid( TOP ).name() << " to " << typeid( X ).name() << " :";
							auto static_cast_result = static_cast< const X* >( &top );
							auto type_id_cast_result = castMethod( c_typed_void, typeid( X ) );
							if( static_cast_result == type_id_cast_result )
								std::cout << "OK";
							else
								std::cout << "ERROR";
							std::cout << std::endl;
						}
					, [&]< typename, typename >	{  }
					});
			}
			, [&]< typename, typename >{}
			});
	}

	void typeid_cast_test()
	{
		std::cout << "\n";
		std::cout << __func__ << "\n";

		using namespace TestDomain;

		{
			virtual_void::domain testDomain;
			virtual_void::typeid_const_cast_method typeid_const_cast( testDomain );
		
			using classes = virtual_void::type_list< D, C1, C2 >;

			virtual_void::fill_const_cast_for( classes{}, typeid_const_cast );


			virtual_void::declare_classes( classes{}, testDomain );
			virtual_void::build_v_tables( testDomain );

			run_cast_test< classes >( typeid_const_cast, []( auto top ){ return virtual_void::to_typed_void( top ); } );

			run_cast_test< classes >( typeid_const_cast, []( auto top ){ return virtual_void::to_virtual_void( top ); } );
		}
	}
}



