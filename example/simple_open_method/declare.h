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
	using typed_const_void = std::pair< const std::type_info&, const void* >;
	using typed_void = std::pair< const std::type_info&, void* >;

	template< typename P > auto to_typed_void( const P* p ){ return typed_const_void{ typeid( P ), p }; }
	template< typename P > auto to_typed_void( P* p ){ return typed_void{ typeid( P ), p }; }

	template< typename >  struct self_pointer;
	template<>  struct self_pointer< void * >		{ template< typename CLASS > using type = CLASS*; };
	template<>  struct self_pointer< const void * >	{ template< typename CLASS > using type = const CLASS*; };

	class error : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	template< typename DISPATCH_TARGET >
	class type_info_dispatch
	{
	public:
		using dispatch_target_t = DISPATCH_TARGET;
	private:
		using method_table_t = std::vector< std::pair< std::type_index, dispatch_target_t > >;;
		method_table_t dispatchTable_;
		gms::Phash_Table phashTable_;
		bool sealed_ = false;
	public:
		auto define_erased( const std::type_info& register_type_info, dispatch_target_t f )
		{
			if( sealed_ )
				throw error( "This open_method is already seald." );
			if( is_defined( register_type_info ) )
				throw error( "Method for type already registered." );
			dispatchTable_.push_back( { std::type_index( register_type_info ), f } );
			return definition{};
		}
		dispatch_target_t is_defined( const std::type_info& type_info ) const
		{
			auto found = std::ranges::find_if( dispatchTable_, [ & ]( const auto& entry ){ return entry.first == type_info; } );
			if( found != dispatchTable_.end() )
				return found->second;
			return nullptr;
		}
		void seal()
		{
			if( dispatchTable_.size() == 1 )
				define_erased( typeid(nullptr_t), nullptr );
			phashTable_ = gms::Phash_Table{ &dispatchTable_, (uint32_t)dispatchTable_.size(), hash_method_build };
			self_test();
			sealed_ = true;
		}
		dispatch_target_t lookup( const std::type_info& type_info ) const
		{
			if( !sealed_ )
				throw error( "Not yet sealed." );
			return lookup_( type_info );
		}
		struct definition{};
	private:
		void self_test() const
		{
			for( std::size_t i = 0; i < dispatchTable_.size(); ++i )
		        lookup_( dispatchTable_[ i ].first );
		}
		dispatch_target_t lookup_( const std::type_info& type_info ) const
		{
			if( !sealed_ )
				throw error( "Not yet sealed." );
			return lookup_( std::type_index{ type_info } );
		}
		dispatch_target_t lookup_( std::type_index type_index ) const
		{
			auto i = phashTable_.lookup( &type_index, hash_method_lookup );
			const auto& found = dispatchTable_.at( i );
			if( found.first != type_index )
				throw error( "phash table corrupt." );
			return found.second;
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

	template< typename R, typename DISPATCH, typename... ARGS >
		requires ( std::same_as< DISPATCH, void* > || std::same_as< DISPATCH, const void* > ) 
	class declare
	{
	public:
		template< typename CLASS > using class_param_t = self_pointer< DISPATCH >::template type< CLASS >;
		using param_t = std::pair< const std::type_info&, DISPATCH >;
		using erased_function_t = R(*)( DISPATCH, ARGS... );
	private:
		type_info_dispatch< erased_function_t > methodTable_;
	public:
		auto define_erased( const std::type_info& ti, erased_function_t f ) { return methodTable_.define_erased( ti, f ); }
		template< typename CLASS, typename FUNCTION >
		auto define( FUNCTION f )
		{
			auto fp = ensure_function_ptr< CLASS >( f );
			return methodTable_.define_erased( typeid( CLASS ), reinterpret_cast< erased_function_t >( fp ) );
		}
		R operator()( const std::type_info& type_info, DISPATCH dispatched, ARGS&&... args ) const
		{
			auto f = methodTable_.lookup( type_info );
			return f( dispatched, std::forward< ARGS >( args )... );
		}
		R operator()( param_t param, ARGS&&... args ) const
		{
			return (*this)( param.first, param.second, std::forward< ARGS >( args )... );
		}
		template< typename CLASS > R operator()( CLASS* param, ARGS&&... args ) const // to simplify tests!
		{
			return (*this)( to_typed_void( param ), std::forward< ARGS >( args )... );
		}
		erased_function_t is_defined( const std::type_info& type_info ) const
		{
			return methodTable_.is_defined( type_info );
		}
		template< typename C > auto is_defined() const
		{
			return is_defined( typeid( C ) );
		}
		void seal() { methodTable_.seal(); }
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
	};

	template< typename R, typename... ARGS >
	class factory;

	template< typename R, typename... ARGS >
	class factory< R( ARGS...) >
	{
	public:
		using factory_function_t = R(*)( ARGS... );
	private:
		type_info_dispatch< factory_function_t > methodTable_;
	public:
		auto define_erased( const std::type_info& ti, factory_function_t f ) { return methodTable_.define_erased( ti, f ); }
		template< typename CLASS, typename FACTORY >
		auto define( FACTORY f )
		{
			auto fp = ensure_factory_ptr< CLASS >( f );
			return methodTable_.define_erased( typeid( CLASS ), reinterpret_cast< factory_function_t >( fp ) );
		}
		R operator()( const std::type_info& type_info, ARGS&&... args ) const
		{
			auto f = methodTable_.lookup( type_info );
			return f( std::forward< ARGS >( args )... );
		}
		template< typename CLASS > R operator()( ARGS&&... args ) const // to simplify tests!
		{
			return (*this)( typeid( CLASS ), std::forward< ARGS >( args )... );
		}
		factory_function_t is_defined( const std::type_info& type_info ) const
		{
			return methodTable_.is_defined( type_info );
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