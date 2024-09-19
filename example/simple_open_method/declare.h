#pragma once

#include <any>
#include <map>
#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>

namespace BitFactory::simple_open_method
{
	template< typename P > auto to_typed_void( const P* p ){ return std::pair< const std::type_info&, const void* >{ typeid( P ), p }; }
	template< typename P > auto to_typed_void( P* p ){ return std::pair< const std::type_info&, void* >{ typeid( P ), p }; }

	template< typename >  struct erased;

	template< typename SELF >  auto self_cast( void* erased )
	{
		return static_cast< SELF* >( erased );
	};
	template< typename SELF >  auto self_cast( const void* erased )
	{
		return static_cast< const SELF* >( erased );
	};

	const std::type_info& get_type_info( const  std::pair< const std::type_info&, const void* >& typed_void ) { return typed_void.first; }
	const std::type_info& get_type_info( const  std::pair< const std::type_info&, void* >& typed_void ) { return typed_void.first; }

	const void* get_erased( std::pair< const std::type_info&, const void* >& typed_void ) { return typed_void.second; }
	void* get_erased( std::pair< const std::type_info&, void* >& typed_void ) { return typed_void.second; }

	class error : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	template< typename R, typename DISPATCH, typename... ARGS >
		requires ( std::same_as< DISPATCH, void* > || std::same_as< DISPATCH, const void* > ) 
	class declare
	{
	public:
		using param_t = std::pair< const std::type_info&, DISPATCH >;
		using erased_function_t = std::function< R( DISPATCH, ARGS... ) >;
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
			return define_erased( typeid( SELF ), [ f ]( DISPATCH erased, ARGS... args )->R
			{
				return f( self_cast< SELF >( erased ), std::forward< ARGS >( args )... );
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