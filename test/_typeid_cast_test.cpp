#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/open_method/via_type_info/open_method.h"
#include "../include/virtual_void/erased/open_method/via_m_table/open_method.h"
#include "../include/virtual_void/typeid_cast/cast.h"
#include "../include/virtual_void/erased/open_method/algorithm.h"
#include "../include/virtual_void/erased/data/has_m_table/observer.h"
#include "../include/virtual_void/erased/data/has_type_info/observer.h"

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
				auto c_typed_void = make_dispatch_var( top );

				virtual_void::class_hierarchy::visit_class< TOP >
					( virtual_void::overload
					{ [&]< typename X >				
						{ 
							auto static_cast_result = static_cast< const X* >( &top );
							auto type_id_cast_result = virtual_void::typeid_cast::cast_to< const X >( castMethod, c_typed_void );
							REQUIRE( static_cast_result == type_id_cast_result );
						}
					, [&]< typename, typename >	{  }
					});
			}
			, [&]< typename, typename >{}
			});
	}

TEST_CASE( "typeid_cast_test" ) 
{
	using namespace TestDomain;
	using namespace virtual_void;

	using classes = virtual_void::type_list< D, C1, C2 >;

	typeid_::domain typeidTestDomain;
	typeid_cast::const_cast_method< typeid_::open_method > typeid_const_cast( typeidTestDomain );
	virtual_void::open_method::declare_classes( classes{}, typeidTestDomain );
	typeid_cast::fill_const_cast_for( classes{}, typeid_const_cast );
	virtual_void::typeid_::seal_for_runtime( typeidTestDomain );

	m_table::domain m_tableTestDomain;
	typeid_cast::const_cast_method< m_table::open_method > m_table_const_cast( m_tableTestDomain );
	virtual_void::open_method::declare_classes( classes{}, m_tableTestDomain );
	virtual_void::m_table::declare_classes( classes{}, m_tableTestDomain );
	typeid_cast::fill_const_cast_for( classes{}, m_table_const_cast );
	virtual_void::m_table::fix_m_tables( m_tableTestDomain );

	run_cast_test< classes >( typeid_const_cast, []( auto& top ){ return erased::data::has_type_info::const_observer( top ); } );

	run_cast_test< classes >( m_table_const_cast, []( auto& top ){ return erased::data::has_m_table::const_observer( top ); } );
}
}



