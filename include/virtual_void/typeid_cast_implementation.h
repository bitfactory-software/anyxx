#pragma once

#include "method.h"
#include "class_hierarchy/class_hierarchy.h"

namespace virtual_void
{
	template< template< typename > typename CONST, typename FOUND, typename FROM > auto typeid_cast_implementation_( auto* from, const std::type_info& to )
	{
		typename CONST< void >::type found = nullptr;
		class_hierarchy::visit_class< FROM >
			( overload
			{ [&]< typename C >				
				{ 
					if( !found&& typeid( FROM ) == to ) 
						found = static_cast< CONST< void >::type >( from ); 
				}
			, [&]< typename, typename B >	
				{	
					if( !found ) 
						found = typeid_cast_implementation_< CONST, FOUND, B >( static_cast< CONST< B >::type >( from ), to ); }
			});
		return found;
	}
	struct typeid_const_cast_implementation
	{
		template< typename T > struct const_ {	using type = const T*; }; 
		template< typename FROM > auto operator()( const FROM* from, const std::type_info& to )
		{
			return typeid_cast_implementation_< const_, typed_const_void, FROM >( from, to );
		}
	};
	struct typeid_cast_implementation
	{
		template< typename T > struct non_const_ { using type = T; }; 
		template< typename FROM > auto operator()( FROM* from, const std::type_info& to )
		{
			return typeid_cast_implementation_< non_const_, typed_void, FROM >( from, to );	
		}
	};
}