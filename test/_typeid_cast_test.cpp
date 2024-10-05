#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "include/catch.hpp"

#include "../include/virtual_void/typeid/cast.h"
#include "../include/virtual_void/open_method/algorithm.h"

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
							auto type_id_cast_result = virtual_void::typeid_::cast_to< const X >( castMethod, c_typed_void );
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

TEST_CASE( "typeid_cast_test" ) 
{
	std::cout << "\n";
	std::cout << __func__ << "\n";

	using namespace TestDomain;
	using namespace virtual_void;

	{
		virtual_void::domain testDomain;
		typeid_::const_cast_method typedid_const_cast( testDomain );
		
		using classes = virtual_void::type_list< D, C1, C2 >;

		typeid_::fill_const_cast_for( classes{}, typedid_const_cast );

		virtual_void::open_method::declare_classes( classes{}, testDomain );
		virtual_void::open_method::build_m_tables( testDomain );

		run_cast_test< classes >( typedid_const_cast, []( auto top ){ return virtual_void::to_typeid_void( top ); } );

		run_cast_test< classes >( typedid_const_cast, []( auto top ){ return virtual_void::to_m_table_void( top ); } );
	}
}
}



