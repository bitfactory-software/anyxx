#pragma once

#include "method.h"
#include "fill_with_overloads.h"

namespace virtual_void
{
	using typeid_const_cast_method = method< const void*( const void*, const std::type_info& to ) >;
	using typeid_cast_method = method< void*( const void*, const std::type_info& to ) >;

	template< typename CLASSES >
	void fill_const_cast_for( typeid_const_cast_method& method )
	{
		fill_with_overloads< CLASSES >( method, typeid_const_cast_implementation{} );
	}

	template< typename CLASSES >
	void fill_cast_for( typeid_cast_method& method )
	{
		fill_with_overloads< CLASSES >( method, typeid_cast_implementation{} );
	}

}