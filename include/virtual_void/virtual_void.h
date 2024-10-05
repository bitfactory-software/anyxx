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

#include "../perfect_typeid_hash/index_table.h"
#include "../utillities/overload.h"
#include "../utillities/type_list.h"

#include "forward.h"

#include "error.h"

#include "class_hierarchy/class_hierarchy.h"
#include "open_method/table.h"

namespace virtual_void
{

//+++open method dispatch
struct domain
{
	class_hierarchy::classes_with_bases		classes;
	std::vector< type_info_dispatch* >		method_dispatches;	
};

class m_table_t
{
public:
	constexpr m_table_t( const std::type_info& type_info )
		: type_info_( type_info )
	{}
	constexpr const std::type_info& type() const { return type_info_; }
	using m_table_target_t = void(*)();
	constexpr void set_method( int method_index, m_table_target_t target )
	{
		ensure_size( method_index + 1  );
		table_[ method_index ] = target;
	}
	template< typename TRAGET >
	constexpr void set_method( int method_index, TRAGET target )
	{
		auto m_table_target = reinterpret_cast< m_table_target_t >( target );
		set_method( method_index, m_table_target );
	}
	constexpr void clear()
	{
		table_.clear();
	}
	constexpr auto at( int method_index ) const
	{
		return table_.at( method_index );
	}
	constexpr auto operator[]( int method_index ) const
	{
		return table_[ method_index ];
	}
private:
	const std::type_info& type_info_;
	std::vector< m_table_target_t > table_;
	constexpr void ensure_size( std::size_t s )
	{
		if( table_.size() >= s )
			return;
		table_.insert( table_.end(), s  - table_.size(), nullptr );
	}
};
template< typename CLASS > constexpr m_table_t* m_table_of()
{
	static m_table_t m_table_{ typeid( CLASS ) };
	return &m_table_;
}

class type_info_dispatch
{
	open_method::table table_;
public:
	using dispatch_target_t = void(*)();
private:
	using entry_t = std::pair< const std::type_info*, dispatch_target_t >;
	using method_table_t = std::vector< entry_t >; // faster than map, slower than hash_map 
	const int m_table_index_ = -1;
	using dispatch_target_index_t  = perfect_typeid_hash::index_table< dispatch_target_t >;
	std::unique_ptr< dispatch_target_index_t > dispatch_target_index_;
public:
	type_info_dispatch() = default;
	type_info_dispatch( domain& domain )
		: m_table_index_( (int)domain.method_dispatches.size() )
	{ 
		domain.method_dispatches.push_back( this ); 
	}
	int m_table_index() const { return m_table_index_; }
	void define_default( auto f ) { table_.define_default( f ); }
	auto get_default() const { return table_.get_default(); }
	auto define_erased( const std::type_info& register_type_info, auto f ) { return table_.define( register_type_info, f ); }
	template< typename TARGET = dispatch_target_t >
	TARGET is_defined( const std::type_info& type_info ) const { return table_.template is_defined< TARGET >( type_info ); }
	template< typename TARGET = dispatch_target_t >
	TARGET lookup( const std::type_info& type_info ) const
	{
		if( !dispatch_target_index_ )
			throw "Not yet sealed.";
		return reinterpret_cast< TARGET >( dispatch_target_index_->at( &type_info ) );
	}
	void seal()
	{
		dispatch_target_index_ = std::make_unique< dispatch_target_index_t >( table_.make_lookup_table() );
	}
};

template< typename R, typename... ARGS >
class method;

template< typename R, typename... ARGS >
class method< R( ARGS... ) >
{
	static_assert 
		(	std::same_as< first< ARGS... >,	void* > 
		||	std::same_as< first< ARGS... >,	const void* > 
		); 
public:
	using dispatch_t = typename first< ARGS... >;
	template< typename CLASS > using class_param_t = self_pointer< dispatch_t >::template type< CLASS >;
	using param_t = std::pair< const std::type_info&, dispatch_t >;
	using virtual_void_t = std::pair< const m_table_t*, dispatch_t >;
	using erased_function_t = R(*)( ARGS... );
private:
	type_info_dispatch methodTable_;
public:
	method( domain& domain )
		: methodTable_( domain )
	{}
	void define_default( auto f )
	{
		methodTable_->define_default( f );
	}
	auto define_erased( const std::type_info& ti, erased_function_t f ) { return methodTable_.define_erased( ti, f ); }
	template< typename CLASS, typename FUNCTION >
	auto define( FUNCTION f )
	{
		auto fp = ensure_function_ptr< CLASS, ARGS... >( f );
		return methodTable_.define_erased( typeid( CLASS ), fp );
	}
	template< typename... OTHER_ARGS >
	R operator()( const std::type_info& type_info, dispatch_t dispatched, OTHER_ARGS&&... args ) const
	{
		auto f = methodTable_.lookup< erased_function_t >( type_info );
		return f( dispatched, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename... OTHER_ARGS >
	R operator()( const param_t& param, OTHER_ARGS&&... args ) const
	{
		return (*this)( param.first, param.second, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename... OTHER_ARGS >
	R operator()( const virtual_void_t& param, OTHER_ARGS&&... args ) const
	{
		const m_table_t& m_table = *param.first;
		auto erased_function = reinterpret_cast< erased_function_t >( m_table[ methodTable_.m_table_index() ] );
		return (erased_function)( param.second, std::forward< OTHER_ARGS >( args )... );
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
	template< typename POINTER, typename... OTHER_ARGS >
	R operator()( const POINTER& pointer, OTHER_ARGS&&... args ) const
		requires MtableDispatchableVoid< POINTER, dispatch_t >
	{
		virtual_void_t param{ pointer.m_table(), pointer.data() }; 
		return (*this)( param, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename POINTER, typename... OTHER_ARGS >
	R call( const POINTER& pointer, OTHER_ARGS&&... args ) const
		requires MtableDispatchableVoid< POINTER, dispatch_t >
	{
		virtual_void_t param{ pointer.m_table(), pointer.data() }; 
		return (*this)( param, std::forward< OTHER_ARGS >( args )... );
	}
	erased_function_t is_defined( const std::type_info& type_info ) const
	{
		return reinterpret_cast< erased_function_t >( methodTable_.is_defined( type_info ) );
	}
	template< typename C > auto is_defined() const
	{
		return is_defined( typeid( C ) );
	}
	void seal()
	{
		return methodTable_.seal();
	}
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

//+++open method algorithms
template< typename CLASS >
void fill_with_overload( auto& method, const auto& wrapper )
{
	if( !method.is_defined< CLASS >() )
		method.define< CLASS >( wrapper );
}
template< typename TYPE_LIST >
void fill_with_overloads( TYPE_LIST, auto& method, const auto& wrapper )
{
	class_hierarchy::visit_classes< TYPE_LIST >( 
		overload
		{ [&]< typename C >				{ fill_with_overload< C >( method, wrapper ); }
		, [&]< typename C, typename B >	{}
		});
}
template< typename... CLASSES >
void fill_with_overloads( auto& method, const auto& wrapper )
{
	fill_with_overloads( type_list< CLASSES... >{}, method, wrapper );
}

inline constexpr auto find_declared_in_bases( const class_hierarchy::classes_with_bases& registry, const class_hierarchy::bases_t& bases, const type_info_dispatch& method )
{
	typename type_info_dispatch::dispatch_target_t found = nullptr;
	visit_bases( bases, registry, [ & ]( const std::type_info& base )
		{
			if( !found )
				found = method.is_defined( base );				
		});
	return found;
}
inline void interpolate( const class_hierarchy::classes_with_bases& classes, type_info_dispatch* method )
{
	for( const auto& [ self, class_with_bases ] : classes )
		if( !method->is_defined( *class_with_bases.self ) )
			if( auto found = find_declared_in_bases( classes, class_with_bases.bases, *method ) )
				method->define_erased( *class_with_bases.self, found );
}
inline void interpolate( const domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		interpolate( domain.classes, method );
}
inline void set_m_table( const class_hierarchy::class_with_bases& class_, const type_info_dispatch& method )
{
	auto target = method.is_defined( *class_.self );
	if( !target )
		target = method.get_default();
	class_.m_table->set_method( method.m_table_index(), target );
}
inline void seal( domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		method->seal();
}
inline void fix_m_tables( const class_hierarchy::classes_with_bases& classes, const type_info_dispatch& method )
{
	for( const auto& class_ : classes )
		set_m_table( class_.second, method );
}
inline void fix_m_tables( const domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		fix_m_tables( domain.classes, *method );
}
template< typename CLASSES > auto declare_classes( CLASSES, domain& domain )
{
	return class_hierarchy::declare_all< CLASSES >( domain.classes );
}
template< typename... CLASSES > auto declare_classes( domain& domain )
{
	return declare_classes( type_list< CLASSES... >{}, domain );
}
inline void build_m_tables( domain& domain )
{
	if( domain.classes.empty() )
		throw error( "no classes declared." );
	interpolate( domain );
	seal( domain );
	fix_m_tables( domain );
}
//---open method algorithms

}