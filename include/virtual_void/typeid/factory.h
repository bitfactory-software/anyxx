#pragma once

#include "../virtual_void.h"

namespace virtual_void::typeid_
{

template< typename R, typename... ARGS >
class factory;

template< typename R, typename... ARGS >
class factory< R( ARGS...) >
{
public:
	using factory_function_t = R(*)( ARGS... );
private:
	type_info_dispatch methodTable_;
public:
	template< typename CLASS, typename FACTORY >
	auto define( FACTORY f )
	{
		auto fp = ensure_factory_ptr< CLASS >( f );
		return methodTable_.define_erased( typeid( CLASS ), reinterpret_cast< factory_function_t >( fp ) );
	}
	R operator()( const std::type_info& type_info, ARGS&&... args ) const
	{
		auto f = methodTable_.lookup< factory_function_t >( type_info );
		return f( std::forward< ARGS >( args )... );
	}
	template< typename CLASS > R operator()( ARGS&&... args ) const // to simplify tests!
	{
		return (*this)( typeid( CLASS ), std::forward< ARGS >( args )... );
	}
	factory_function_t is_defined( const std::type_info& type_info ) const
	{
		return methodTable_.is_defined< factory_function_t >( type_info );
	}
	template< typename C > auto is_defined() const
	{
		return is_defined( typeid( C ) );
	}
	void seal() { methodTable_.seal(); }
private:
	template< typename CLASS >
	static auto ensure_factory_ptr( auto functor ) // if functor is a templated operator() from a stateless function object, instantiate it now!;
	{
		using functor_t = decltype( functor );
		if constexpr( std::is_pointer_v< functor_t > )
		{
			return functor;
		}
		else
		{
			return +[]( ARGS&&... args )->R
			{
				return functor_t{}. template operator()< CLASS >( std::forward< ARGS >( args )... );
			};
		}
	}
};

}