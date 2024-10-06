#pragma once

#include <typeindex>

#include "../../utillities/overload.h"
#include "../../utillities/type_list.h"
#include "../../utillities/ensure_function_ptr.h"

#include "open_method_base.h"

namespace virtual_void::typeid_
{

template< typename R, typename... ARGS > class open_method;
template< typename R, typename... ARGS > class open_method< R( ARGS... ) > 
	: public open_method_base
{
	static_assert 
		(	std::same_as< first< ARGS... >,	void* > 
		||	std::same_as< first< ARGS... >,	const void* > 
		); 
public:
	using dispatch_t = typename first< ARGS... >;
	template< typename CLASS > using class_param_t = self_pointer< dispatch_t >::template type< CLASS >;
	using param_t = std::pair< const std::type_info&, dispatch_t >;
	using erased_function_t = R(*)( ARGS... );
public:
	using open_method_base::open_method_base;
	template< typename CLASS, typename FUNCTION >
	auto define( FUNCTION f )
	{
		if( dispatch_target_index_ )
			throw error( "Already sealed for runtime." );
		auto fp = ensure_function_ptr< CLASS, class_param_t, R, ARGS... >( f );
		return define_erased( typeid( CLASS ), fp );
	}
	template< typename... OTHER_ARGS >
	R operator()( const std::type_info& type_info, dispatch_t dispatched, OTHER_ARGS&&... args ) const
	{
		auto f = lookup< erased_function_t >( type_info );
		return f( dispatched, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename... OTHER_ARGS >
	R operator()( const param_t& param, OTHER_ARGS&&... args ) const
	{
		return (*this)( param.first, param.second, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename CLASS, typename... OTHER_ARGS >
	R operator()( CLASS* param, OTHER_ARGS&&... args ) const
	{
		return (*this)( to_typeid_void( param ), std::forward< OTHER_ARGS >( args )... );
	}
	template< typename CLASS, typename... OTHER_ARGS >
	R operator()( const std::shared_ptr< CLASS >& param, OTHER_ARGS&&... args ) const
	{
		return (*this)( param.get(), std::forward< OTHER_ARGS >( args )... );
	}
};

}