#pragma once

#include <any>
#include <map>
#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>

#include "phash/phash_table.hh"

namespace BitFactory::simple_open_method
{
	template< typename P > auto to_typed_void( const P* p ){ return std::pair< const std::type_info&, const void* >{ typeid( P ), p }; }
	template< typename P > auto to_typed_void( P* p ){ return std::pair< const std::type_info&, void* >{ typeid( P ), p }; }

	template< typename >  struct self_pointer;
	template<>  struct self_pointer< void * >		{ template< typename CLASS > using type = CLASS*; };
	template<>  struct self_pointer< const void * >	{ template< typename CLASS > using type = const CLASS*; };

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
		template< typename CLASS > using class_param_t = self_pointer< DISPATCH >::template type< CLASS >;
		using param_t = std::pair< const std::type_info&, DISPATCH >;
		using erased_function_t = R(*)( DISPATCH, ARGS... );
	private:
		using method_table_t = std::vector< std::pair< std::type_index, erased_function_t > >;;
		method_table_t methodTable_;
		gms::Phash_Table phashTable_;
		bool sealed_ = false;
	public:
		auto define_erased( const std::type_info& register_type_info, erased_function_t f )
		{
			if( sealed_ )
				throw error( "This open_method is already seald." );
			if( is_defined( register_type_info ) )
				throw error( "Method for type already registered." );
			methodTable_.push_back( { std::type_index( register_type_info ), f } );
			return definition{};
		}
		template< typename CLASS, typename FUNCTION >
		auto define( FUNCTION f )
		{
			auto fp = ensure_function_ptr< CLASS >( f );
			return define_erased( typeid( CLASS ), reinterpret_cast< erased_function_t >( fp ) );
		}
		R operator()( param_t param, ARGS&&... args ) const
		{
			if( !sealed_ )
				throw error( "Not yet sealed." );
			auto f = lookup( param.first );
			return f( param.second, std::forward< ARGS >( args )... );
		}
		erased_function_t lookup( const std::type_info& type_info ) const
		{
			return lookup( std::type_index{ type_info } );
		}
		erased_function_t lookup( std::type_index type_index ) const
		{
			auto i = phashTable_.lookup( &type_index, hash_method_lookup );
			const auto& found = methodTable_.at( i );
			if( found.first != type_index )
				throw error( "phash table corrupt." );
			return found.second;
		}
		erased_function_t is_defined( const std::type_info& type_info ) const
		{
			auto found = std::ranges::find_if( methodTable_, [ & ]( const auto& entry ){ return entry.first == type_info; } );
			if( found != methodTable_.end() )
				return found->second;
			return nullptr;
		}
		template< typename C >
		auto is_defined() const
		{
			return is_defined( typeid( C ) );
		}
		void seal()
		{
			if( methodTable_.size() == 1 )
				define_erased( typeid(nullptr_t), nullptr );
			phashTable_ = gms::Phash_Table{ &methodTable_, (uint32_t)methodTable_.size(), hash_method_build };
			self_test();
			sealed_ = true;
		}
		struct definition{};
	private:
		template< typename CLASS >
		static auto ensure_function_ptr( auto functor ) // if functor is a templated operator() from a stateless function object, instantiate it now!;
		{
			using functor_t = decltype( functor );
			if constexpr( std::is_pointer_v< functor_t > )
			{
				return functor;
			}
			else
			{
				return +[]( class_param_t< CLASS > self, ARGS&&... args )->R
				{
					return functor_t{}( self, std::forward< ARGS >( args )... );
				};
			}
		}
		void self_test() const
		{
			for( std::size_t i = 0; i < methodTable_.size(); ++i )
		        lookup( methodTable_[ i ].first );
		}
		static auto hash_method_build( const void *p, uint32_t i, uint32_t param )->uint32_t
		{
			auto method_table = static_cast< const method_table_t* >( p );
			auto& method = method_table->at( i );
			return hash_method_typed( &method.first, 0, param );
		}
		static auto hash_method_lookup( const void* type_index, uint32_t, uint32_t param )->uint32_t
		{
			return hash_method_typed( static_cast< const std::type_index* >( type_index ), sizeof( type_index ), param );
		}
		static auto hash_method_typed( const std::type_index* type_index, uint32_t, uint32_t param )->uint32_t
		{
			return gms_hash_sdbm_32( type_index, sizeof( type_index ), param );
		}
	};
}