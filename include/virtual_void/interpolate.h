#pragma once

#include "class_hierarchy/class_hierarchy.h"

namespace virtual_void
{
	template< typename DEFINITION >
	DEFINITION::erased_function_t find_decared_in_bases( DEFINITION& method, const class_hierarchy::classes_with_bases& registry, const class_hierarchy::bases_t& bases )
	{
		typename DEFINITION::erased_function_t found = nullptr;
		visit_bases( bases, registry, [ & ]( const std::type_info& base )
			{
				if( !found )
					found = method.is_defined( base );				
			});
		return found;
	}

	template< typename DEFINITION >
	void interpolate( DEFINITION& method, const class_hierarchy::classes_with_bases& registry )
	{
		for( const auto& [ self, class_with_bases ] : registry )
			if( !method.is_defined( *class_with_bases.self ) )
				if( auto found = find_decared_in_bases( method, registry, class_with_bases.bases ) )
					method.override_erased( *class_with_bases.self, found );
	}
}	