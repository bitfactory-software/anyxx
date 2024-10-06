#pragma once

#include <typeindex>

#include "../../utillities/overload.h"
#include "../../utillities/type_list.h"

#include "../open_method/table.h"
#include "../class_hierarchy/class_hierarchy.h"
#include "m_table.h"

namespace virtual_void::m_table
{

class open_method_base;
using open_methods = std::vector< open_method_base* >;

using m_table_map = std::unordered_map< std::type_info const *, m_table_t* >;

class open_method_base : public virtual_void::open_method::table
{
	const int m_table_index_ = -1;
public:
	explicit open_method_base( open_methods& domain )
		: m_table_index_( (int)domain.size() )
	{ 
		domain.push_back( this ); 
	}		
	int m_table_index() const { return m_table_index_; }
};

template< typename R, typename... ARGS >
class open_method;

template< typename R, typename... ARGS >
class open_method< R( ARGS... ) > : public open_method_base
{
	static_assert 
		(	std::same_as< first< ARGS... >,	void* > 
		||	std::same_as< first< ARGS... >,	const void* > 
		); 
public:
	using dispatch_t = typename first< ARGS... >;
	template< typename CLASS > using class_param_t = self_pointer< dispatch_t >::template type< CLASS >;
	using virtual_void_t = std::pair< const m_table_t*, dispatch_t >;
	using erased_function_t = R(*)( ARGS... );
private:
	table table_;
public:
	using open_method_base::open_method_base;
	template< typename CLASS, typename FUNCTION >
	auto define( FUNCTION f )
	{
		auto fp = ensure_function_ptr< CLASS, ARGS... >( f );
		return define_erased( typeid( CLASS ), fp );
	}
	template< typename... OTHER_ARGS >
	R operator()( const virtual_void_t& param, OTHER_ARGS&&... args ) const
	{
		const m_table_t& m_table = *param.first;
		auto erased_function = reinterpret_cast< erased_function_t >( m_table[ m_table_index() ] );
		return (erased_function)( param.second, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename POINTER, typename... OTHER_ARGS >
	R operator()( const POINTER& pointer, OTHER_ARGS&&... args ) const
		requires MtableDispatchableVoid< POINTER, dispatch_t >
	{
		return call( pointer, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename POINTER, typename... OTHER_ARGS >
	R call( const POINTER& pointer, OTHER_ARGS&&... args ) const
		requires MtableDispatchableVoid< POINTER, dispatch_t >
	{
		virtual_void_t param{ pointer.m_table(), pointer.data() }; 
		return (*this)( param, std::forward< OTHER_ARGS >( args )... );
	}
private:
	template< typename CLASS, typename DISPATCH, typename... OTHER_ARGS >
	static auto ensure_function_ptr( auto functor ) // if functor is a templated operator() from a stateless function object, instantiate it now!;
	{
		using functor_t = decltype( functor );
		if constexpr( std::is_pointer_v< functor_t > )
		{
			return functor;
		}
		else
		{
			return +[]( class_param_t< CLASS > self, OTHER_ARGS&&... args )->R
			{
				return functor_t{}( self, std::forward< OTHER_ARGS >( args )... );
			};
		}
	}
};

template< typename CLASSES >
constexpr nullptr_t register_m_tables( m_table_map& registry )
{
	class_hierarchy::visit_classes< CLASSES >
		( overload
		{ [&]< typename C >{ registry[ &typeid( C ) ] = m_table_of< C >(); }
		, [&]< typename C, typename B >	{}
		});	
	return {};
}

inline void set_m_table( auto type_info, auto m_table, const open_method_base& method )
{
	auto target = method.is_defined( *type_info );
	if( !target )
		target = method.get_default();
	m_table->set_method( method.m_table_index(), target );
}
inline void fix_m_tables( const m_table_map& m_table_map, const open_method_base& method )
{
	for( auto [ type_info, m_tabele ] : m_table_map )
		set_m_table( type_info, m_tabele, method );
}
inline void fix_m_tables( const open_methods& domain, const m_table_map& m_table_map )
{
	for( const auto& method : domain )
		fix_m_tables( m_table_map, *method );
}

}