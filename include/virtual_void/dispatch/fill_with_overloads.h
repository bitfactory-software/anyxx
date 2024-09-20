#pragma once

#include "../class_hierarchy/class_hierarchy.h"

namespace virtual_void::dispatch
{

	template< typename CLASS, typename DEFINITION >
	void fill_with_overload( DEFINITION& method, auto wrapper )
	{
		if( !method.is_defined< CLASS >() )
			method.define< CLASS >( wrapper );
	}

	template< typename CLASSES, typename DEFINITION >
	void fill_with_overloads( DEFINITION& method, auto wrapper )
	{
		class_hierarchy::visit_classes< CLASSES >( 
			overload
			{ [&]< typename C >				{ fill_with_overload< C >( method, wrapper ); }
			, [&]< typename C, typename B >	{}
			});
	}
}