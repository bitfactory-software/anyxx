#pragma once

#include <any>
#include <map>
#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <algorithm>

#include "../utilities/type_list.h"

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
		using entry_t = std::pair< std::type_index, dispatch_target_t >;
		using method_table_t = std::vector< entry_t >;;
		method_table_t dispatchTable_;
		dispatch_target_t default_ = reinterpret_cast< dispatch_target_t >( &throw_not_found );
		bool sealed_ = false;
	public:
		static void throw_not_found(){ throw error( "no target specified." ); }
		auto define_default( dispatch_target_t f )
		{
			default_ = dispatch_target_t;
		}
		auto get_default() const
		{
			return default_;
		}
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
			auto found = std::find_if( dispatchTable_.begin(), dispatchTable_.end(), [ & ]( const entry_t& entry ){ return entry.first == type_info; } );
			if( found != dispatchTable_.end() )
				return found->second;
			return nullptr;
		}
		void seal()
		{
			std::ranges::sort( dispatchTable_, l_lower_r_ );
			self_test();
			sealed_ = true;
		}
		dispatch_target_t lookup( const std::type_info& type_info ) const
		{
			if( !sealed_ )
				throw error( "Not yet sealed." );
			auto found = lookup_( type_info );
			if( found == dispatchTable_.end() )
				return default_;
			return found->second;
		}
		struct definition{};
	private:
		static auto l_lower_r_( const entry_t& l, const entry_t& r ){ return l.first < r.first; };
		void self_test() const
		{
			for( std::size_t i = 0; i < dispatchTable_.size(); ++i )
		        if( auto found = lookup_( dispatchTable_[ i ].first );
					found == dispatchTable_.end() || found->first != dispatchTable_[ i ].first
					)
					throw error( "dispatchTable_ corrupt." );
		}
		auto lookup_( const std::type_info& type_info ) const
		{
			if( !sealed_ )
				throw error( "Not yet sealed." );
			return lookup_( std::type_index{ type_info } );
		}
		auto lookup_( std::type_index type_index ) const
		{
			return std::lower_bound( dispatchTable_.begin(), dispatchTable_.end(), entry_t{ type_index, nullptr }, l_lower_r_ );
		}
	};

	template< typename R, typename... ARGS >
	class declare;

	template< typename R, typename... ARGS >
	class declare< R( ARGS... ) >
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
	private:
		type_info_dispatch< erased_function_t > methodTable_;
	public:
		auto define_erased( const std::type_info& ti, erased_function_t f ) { return methodTable_.define_erased( ti, f ); }
		template< typename CLASS, typename FUNCTION >
		auto define( FUNCTION f )
		{
			auto fp = ensure_function_ptr< CLASS, ARGS... >( f );
			return methodTable_.define_erased( typeid( CLASS ), reinterpret_cast< erased_function_t >( fp ) );
		}
		template< typename... OTHER_ARGS >
		R operator()( const std::type_info& type_info, dispatch_t dispatched, OTHER_ARGS&&... args ) const
		{
			auto f = methodTable_.lookup( type_info );
			return f( dispatched, std::forward< OTHER_ARGS >( args )... );
		}
		template< typename... OTHER_ARGS >
		R operator()( const param_t& param, OTHER_ARGS&&... args ) const
		{
			return (*this)( param.first, param.second, std::forward< OTHER_ARGS >( args )... );
		}
		template< typename CLASS, typename... OTHER_ARGS >
		R operator()( CLASS* param, OTHER_ARGS&&... args ) const // to simplify tests!
		{
			return (*this)( to_typed_void( param ), std::forward< OTHER_ARGS >( args )... );
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