#pragma once

#include <typeindex>

#include "../../utillities/overload.h"
#include "../../utillities/type_list.h"
#include "../../utillities/ensure_function_ptr.h"

#include "../forward.h"
#include "../open_method/table.h"
#include "../open_method/domain.h"

namespace virtual_void::typeid_
{

class open_method_base;
using domain = virtual_void::open_method::domain< open_method_base >;

class open_method_base : public virtual_void::open_method::table
{
protected:
	using dispatch_target_index_t  = perfect_typeid_hash::index_table< dispatch_target_t >;
	std::unique_ptr< dispatch_target_index_t > dispatch_target_index_;
public:
	explicit open_method_base( domain& domain )
	{ 
		domain.open_methods.push_back( this ); 
	}		
	void seal_for_runtime()
	{
		dispatch_target_index_ = std::make_unique< dispatch_target_index_t >( make_lookup_table() );
	}
};

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
		auto f = lookup( type_info );
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
	void seal_for_runtime()
	{
		dispatch_target_index_ = std::make_unique< dispatch_target_index_t >( make_lookup_table() );
	}
	auto lookup( const std::type_info& type_info ) const
	{
		if( !dispatch_target_index_ )
			throw error( "Not yet sealed for runtime." );
		return reinterpret_cast< erased_function_t >( dispatch_target_index_->at( &type_info ) );
	}
};

inline void seal_for_runtime( domain& domain )
{
	for( const auto& method : domain.open_methods )
		method->seal_for_runtime();
}

}