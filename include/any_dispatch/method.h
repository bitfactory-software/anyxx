#pragma once

#include <any>
#include <functional>
#include <map>
#include <typeindex>

namespace any_dispatch
{
    template< typename R, typename ANY, typename... ARGS >
    class method
    {
		using erased_function_t = std::function< R( ANY, ARGS... ) >;
	public:
		std::map< std::type_index, erased_function_t > dispatchTable_;
        erased_function_t fallback_ = []( ANY dispatch, ARGS... args )->R
        {
			throw std::runtime_error( std::string( dispatch.type().name() ) + ": No registered method." );
        };
	public:
		auto define_erased( const std::type_info& register_type_info, const erased_function_t& f )
		{
			auto entry = dispatchTable_.find( register_type_info );
			if( entry != dispatchTable_.end() )
				throw std::runtime_error( "Method for type already registered." );
			dispatchTable_[ register_type_info ] = f;
			return nullptr;
		}
		auto define_fallback( const erased_function_t& fallback )
		{
            fallback_ = fallback;
		}
        template< typename DISPATCH >
		auto define( const auto& f )
		{
			return define_erased( typeid( DISPATCH ), [ f ]( ANY dispatch, ARGS... args )->R
			{
                auto unerased = std::any_cast< DISPATCH >( &dispatch );   
				return f( unerased, std::forward< ARGS >( args )... );
			});
		}
		R operator()( ANY dispatch, ARGS... args ) const
		{
			auto entry = dispatchTable_.find( dispatch.type() );
			if( entry == dispatchTable_.end() )
				return fallback_( dispatch, args... );
			return entry->second( dispatch, args... );
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
    };
}

