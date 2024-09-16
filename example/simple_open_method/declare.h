#pragma once

#include <any>
#include <map>
#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>

#include "error.h"

namespace BitFactory::simple_open_method
{
	template< typename P > auto to_typed_void( const P* p ){ return std::pair< const std::type_info&, const void* >{ typeid( P ), p }; }
	template< typename P > auto to_typed_void( P* p ){ return std::pair< const std::type_info&, void* >{ typeid( P ), p }; }

	template< typename >  struct erased;
	template< typename, typename >  struct unerased;
	template<>  struct erased< const std::any >
	{
		using type = const std::any&;
		using param = const std::any&;
	};
	template< typename SELF >  struct unerased< SELF, const std::any >
	{
		using type = const SELF*;
		auto operator()( const std::any& erased ) const { return std::any_cast< const SELF >( &erased ); };
	};
	template<>  struct erased< std::any >
	{
		using type = std::any;
		using param = std::any&;
	};
	template< typename SELF > struct unerased< SELF, std::any >
	{
		using type = SELF*;
		auto operator()( std::any& erased ) const { return std::any_cast< SELF >( &erased ); };
	};
	template<>  struct erased< const void >
	{
		using type = const void*;
		using param = std::pair< const std::type_info&, type >;
	};
	template< typename SELF >  struct unerased< SELF, const void >
	{
		using type = const SELF*;
		auto operator()( const void* erased ){ return reinterpret_cast< type >( erased ); };
	};
	template<>  struct erased< void >
	{
		using type = void*;
		using param = std::pair< const std::type_info&, type >;
	};
	template< typename SELF >  struct unerased< SELF, void >
	{
		using type = SELF*;
		auto operator()( void* erased ){ return reinterpret_cast< type >( erased ); };
	};

	const std::type_info& get_type_info( const std::any& any ) { return any.type(); }
	const std::type_info& get_type_info( const  std::pair< const std::type_info&, const void* >& typed_void ) { return typed_void.first; }
	const std::type_info& get_type_info( const  std::pair< const std::type_info&, void* >& typed_void ) { return typed_void.first; }
	const std::any get_erased( const std::any& any ) { return any; }
	std::any get_erased( std::any& any ) { return any; }
	const void* get_erased( std::pair< const std::type_info&, const void* >& typed_void ) { return typed_void.second; }
	void* get_erased( std::pair< const std::type_info&, void* >& typed_void ) { return typed_void.second; }

	template< typename R, typename DISPATCH, typename... ARGS >
	class declare
	{
	public:
		using erased_t = erased< DISPATCH >::type;
		using param_t = erased< DISPATCH >::param;
		using erased_function_t = std::function< R( erased_t, ARGS... ) >;
	private:
		std::map< std::type_index, erased_function_t > methodTable_; 
	public:
		auto define_erased( const std::type_info& register_type_info, const erased_function_t& f )
		{
			auto entry = methodTable_.find( register_type_info );
			if( entry != methodTable_.end() )
				throw error( "Method for type already registered." );
			methodTable_[ register_type_info ] = f;
			return definition{};
		}
		template< typename SELF >
		auto define( const auto& f )
		{
			return define_erased( typeid( SELF ), [ f ]( erased_t erased, ARGS... args )->R
			{
				return f( unerased< SELF, DISPATCH >{}( erased ), std::forward< ARGS >( args )... );
			});
		}
		R operator()( param_t param, ARGS&&... args ) const
		{
			auto entry = methodTable_.find( get_type_info( param ) );
			if( entry == methodTable_.end() )
				throw error( "No registered method." );
			return entry->second( get_erased( param ), std::forward< ARGS >( args )... );
		}	
		erased_function_t is_defined( const std::type_info& type_info )
		{
			if( auto entry = methodTable_.find( type_info ); entry != methodTable_.end() )
				return entry->second;
			return nullptr_t{};
		}
		template< typename C >
		erased_function_t is_defined()
		{
			return is_defined( typeid( C ) );
		}
		class definition{};
	};
}