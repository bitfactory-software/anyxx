#pragma once

#include "../virtual_void.h"

namespace virtual_void
{

//+++open method algorithms
template< typename CLASS >
void fill_with_overload( auto& method, const auto& wrapper )
{
	if( !method.is_defined< CLASS >() )
		method.define< CLASS >( wrapper );
}
template< typename TYPE_LIST >
void fill_with_overloads( TYPE_LIST, auto& method, const auto& wrapper )
{
	class_hierarchy::visit_classes< TYPE_LIST >( 
		overload
		{ [&]< typename C >				{ fill_with_overload< C >( method, wrapper ); }
		, [&]< typename C, typename B >	{}
		});
}
template< typename... CLASSES >
void fill_with_overloads( auto& method, const auto& wrapper )
{
	fill_with_overloads( type_list< CLASSES... >{}, method, wrapper );
}

inline constexpr auto find_declared_in_bases( const class_hierarchy::classes_with_bases& registry, const class_hierarchy::bases_t& bases, const type_info_dispatch& method )
{
	typename type_info_dispatch::dispatch_target_t found = nullptr;
	visit_bases( bases, registry, [ & ]( const std::type_info& base )
		{
			if( !found )
				found = method.is_defined( base );				
		});
	return found;
}
inline void interpolate( const class_hierarchy::classes_with_bases& classes, type_info_dispatch* method )
{
	for( const auto& [ self, class_with_bases ] : classes )
		if( !method->is_defined( *class_with_bases.self ) )
			if( auto found = find_declared_in_bases( classes, class_with_bases.bases, *method ) )
				method->define_erased( *class_with_bases.self, found );
}
inline void interpolate( const domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		interpolate( domain.classes, method );
}
inline void set_m_table( const class_hierarchy::class_with_bases& class_, const type_info_dispatch& method )
{
	auto target = method.is_defined( *class_.self );
	if( !target )
		target = method.get_default();
	class_.m_table->set_method( method.m_table_index(), target );
}
inline void seal( domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		method->seal();
}
inline void fix_m_tables( const class_hierarchy::classes_with_bases& classes, const type_info_dispatch& method )
{
	for( const auto& class_ : classes )
		set_m_table( class_.second, method );
}
inline void fix_m_tables( const domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		fix_m_tables( domain.classes, *method );
}
template< typename CLASSES > auto declare_classes( CLASSES, domain& domain )
{
	return class_hierarchy::declare_all< CLASSES >( domain.classes );
}
template< typename... CLASSES > auto declare_classes( domain& domain )
{
	return declare_classes( type_list< CLASSES... >{}, domain );
}
inline void build_m_tables( domain& domain )
{
	if( domain.classes.empty() )
		throw error( "no classes declared." );
	interpolate( domain );
	seal( domain );
	fix_m_tables( domain );
}

}