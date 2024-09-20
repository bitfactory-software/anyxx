#pragma once

#include <vector>
#include <map>
#include <typeindex>
#include <stdexcept>

#include "class_hierarchy/class_hierarchy.h"
#include "utilities/type_list.h"

namespace virtual_void
{
	template < typename CLASS >
	void clear_v_table()
	{
		v_table_of< CLASS >()->clear();
	}

	template < typename CLASSES, typename METHOD >
	void clear_v_tables()
	{
		class_hierarchy::visit_classes
			( overload
			{ [&]< typename C >				{ clear_v_table< C >(); }
			, [&]< typename C, typename B >	{}
			});
	}

	template < typename CLASS, typename METHOD >
	void set_v_table( const METHOD& method )
	{
		auto target = method.is_defined< CLASS >();
		if( !target )
			throw std::logic_error("No dispatch target found.");
		v_table_of< CLASS >()->set_method( method.v_table_index(), target );
	}

	template < typename CLASSES, typename METHOD >
	void build_v_tables( const METHOD& method )
	{
		class_hierarchy::visit_classes< CLASSES >
			( overload
			{ [&]< typename C >				{ set_v_table< C >( method ); }
			, [&]< typename C, typename B >	{}
			});
	}
}	