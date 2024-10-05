#pragma once

#include "../virtual_void.h"
#include "../open_method/algorithm.h"

namespace virtual_void::typeid_
{

template< template< typename > typename CONST, typename FOUND, typename FROM > auto cast_implementation_( auto* from, const std::type_info& to )
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
					found = cast_implementation_< CONST, FOUND, B >( static_cast< CONST< B >::type >( from ), to ); }
		});
	return found;
}
struct const_cast_implementation
{
	template< typename T > struct const_ {	using type = const T*; }; 
	template< typename FROM > auto operator()( const FROM* from, const std::type_info& to )
	{
		return cast_implementation_< const_, typeid_const_void, FROM >( from, to );
	}
};
struct cast_implementation
{
	template< typename T > struct non_const_ { using type = T; }; 
	template< typename FROM > auto operator()( FROM* from, const std::type_info& to )
	{
		return cast_implementation_< non_const_, typeid_void, FROM >( from, to );	
	}
};
using const_cast_method = method< const void*( const void*, const std::type_info& to ) >;
using cast_method = method< void*( const void*, const std::type_info& to ) >;
void fill_const_cast_for( auto classes, const_cast_method& method )
{
	virtual_void::open_method::fill_with_overloads( classes, method, const_cast_implementation{} );
}
template< typename... CLASSES >
void fill_const_cast_for( const_cast_method& method )
{
	fill_const_cast_for( type_list< CLASSES... >{}, method );
}
void fill_cast_for( auto classes, cast_method& method )
{
	fill_with_overloads( classes, method, cast_implementation{} );
}
template< typename... CLASSES >
void fill_cast_for( cast_method& method )
{
	fill_cast_for( type_list< CLASSES... >{}, method );
}
template< typename TO >
auto cast_to( const const_cast_method& cast, const auto& from )
{
    if( auto void_ = cast( from, typeid( std::remove_const_t< TO > ) ) )
        return static_cast< TO* >( void_ );
	return static_cast< TO* >( nullptr );
}    

}