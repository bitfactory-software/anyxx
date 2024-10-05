#pragma once

#include "../virtual_void.h"

namespace virtual_void::typeid_
{

template< template< typename > typename CONST, typename FOUND, typename FROM > auto erased_cast_implementation_( auto* from, const std::type_info& to )
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
					found = erased_cast_implementation_< CONST, FOUND, B >( static_cast< CONST< B >::type >( from ), to ); }
		});
	return found;
}
struct erased_const_cast_implementation
{
	template< typename T > struct const_ {	using type = const T*; }; 
	template< typename FROM > auto operator()( const FROM* from, const std::type_info& to )
	{
		return erased_cast_implementation_< const_, typeid_const_void, FROM >( from, to );
	}
};
struct erased_cast_implementation
{
	template< typename T > struct non_const_ { using type = T; }; 
	template< typename FROM > auto operator()( FROM* from, const std::type_info& to )
	{
		return erased_cast_implementation_< non_const_, typeid_void, FROM >( from, to );	
	}
};
using erased_const_cast_method = method< const void*( const void*, const std::type_info& to ) >;
using erased_cast_method = method< void*( const void*, const std::type_info& to ) >;
void fill_const_cast_for( auto classes, erased_const_cast_method& method )
{
	fill_with_overloads( classes, method, erased_const_cast_implementation{} );
}
template< typename... CLASSES >
void fill_const_cast_for( erased_const_cast_method& method )
{
	fill_const_cast_for( type_list< CLASSES... >{}, method );
}
void fill_cast_for( auto classes, erased_cast_method& method )
{
	fill_with_overloads( classes, method, erased_cast_implementation{} );
}
template< typename... CLASSES >
void fill_cast_for( erased_cast_method& method )
{
	fill_cast_for( type_list< CLASSES... >{}, method );
}
template< typename TO >
auto cast_to( const erased_const_cast_method& cast, const auto& from )
{
    if( auto void_ = cast( from, typeid( std::remove_const_t< TO > ) ) )
        return static_cast< TO* >( void_ );
	return static_cast< TO* >( nullptr );
}    

}