#pragma once

#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <assert.h>

#include "../virtual_void/virtual_void.h"

namespace any_dispatch
{
template< typename R, typename ANY, typename... ARGS >
class method
{
public:
	virtual_void::type_info_dispatch dispatchTable_;
	using dispatch_target_t = virtual_void::type_info_dispatch::dispatch_target_t;
    erased_function_t fallback_ = []( ANY dispatch, ARGS... args )->R
    {
		throw std::runtime_error( std::string( dispatch.type().name() ) + ": No registered method." );
    };
public:
	auto define_fallback( const erased_function_t& fallback )
	{
        fallback_ = fallback;
	}
    template< typename DISPATCH >
	auto define( const auto& f )
	{
		auto fp = ensure_function_ptr< CLASS, ARGS... >( f );
		return dispatchTable_.override_erased( typeid( CLASS ), +[]( dispatch_target_t erased_target, ANY any, ARGS&&... args )->R
			{
				auto unerased_target = reinterpret_cast< R(*)( ANY, ARGS ) >( erased_target );
				auto self = std::any_cast< CLASS* >( any );
				return unerased_target( self, std::forward< ARGS >( args ) );
			});
	}
	R operator()( ANY dispatch, ARGS... args ) const
	{
		auto f = dispatchTable_.lookup( type_info );

		return f( dispatched, std::forward< OTHER_ARGS >( args )... );
	}	
	erased_function_t is_defined( const std::type_info& type_info )
	{
		if( auto entry = dispatchTable_.find( type_info ); entry != dispatchTable_.end() )
			return entry->second;
		return nullptr_t{};
	}
	template< typename C >
	erased_function_t is_defined()
	{
		return is_defined( typeid( C ) );
	}
private:
	template< typename CLASS, typename ANY, typename... ARGS >
	static auto ensure_function_ptr( auto functor ) // if functor is a templated operator() from a stateless function object, instantiate it now!;
	{
		using functor_t = decltype( functor );
		if constexpr( std::is_pointer_v< functor_t > )
		{
			return functor;
		}
		else
		{
								
			return +[]( CLASS* self, OTHER_ARGS&&... args )->R
			{
				return functor_t{}( self, std::forward< OTHER_ARGS >( args )... );
			};
		}
	}
};

}