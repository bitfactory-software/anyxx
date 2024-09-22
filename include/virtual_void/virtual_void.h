#pragma once

#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <vector>

namespace virtual_void
{
// ++utillities
	template<typename ... Ts> struct overload : Ts ... { using Ts::operator() ...; };
	template<class... Ts> overload(Ts...) -> overload<Ts...>;

	template< typename... ARGS >
	struct type_list
	{
		// see https://vittorioromeo.info/index/blog/cpp20_lambdas_compiletime_for.html
		static void for_each( auto&& call )
		{
			( call.template operator()< ARGS >(), ... );
		}
	};	

	template<int N, typename... Ts> using nth_type =
		typename std::tuple_element<N, std::tuple<Ts...>>::type;

	template< typename... Ts> using first = nth_type< 0, Ts...>;
// --utillities

class v_table;
template< typename CLASS > constexpr v_table* v_table_of();

using virtual_const_void = std::pair< const v_table*, const void* >;
using virtual_void = std::pair< const v_table*, void* >;

template< typename P > auto to_virtual_void( const P* p ){ return virtual_const_void{ v_table_of< P >(), p }; }
template< typename P > auto to_virtual_void( P* p ){ return virtual_const_void{ v_table_of< P >(), p }; }

using typed_const_void = std::pair< const std::type_info&, const void* >;
using typed_void = std::pair< const std::type_info&, void* >;

template< typename P > auto to_typed_void( const P* p ){ return typed_const_void{ typeid( P ), p }; }
template< typename P > auto to_typed_void( P* p ){ return typed_void{ typeid( P ), p }; }

template< typename >  struct self_pointer;
template<>  struct self_pointer< void * >		{ template< typename CLASS > using type = CLASS*; };
template<>  struct self_pointer< const void * >	{ template< typename CLASS > using type = const CLASS*; };

class type_info_dispatch;

template< typename DISPATCH, typename VOID >
concept VtableDispatchableVoid = requires( const DISPATCH& void_ )
{
    { void_.data() }	-> std::convertible_to< VOID >;
    { void_.v_table() } -> std::same_as< v_table* >;
};

namespace class_hierarchy
{
	template< typename CLASS > struct describe;

	template< typename... BASES > using are = type_list< BASES... >;
	using none = type_list<>;

	template< typename CLASS, bool deep = true >
	constexpr void visit_class( auto visitor )
	{
		visitor.template operator()< CLASS >();
		using bases = describe< CLASS >::bases;
		bases::for_each( [ & ]< typename BASE >()
		{ 
			visitor.template operator()< CLASS, BASE >();
			if constexpr( deep )
			{
				visit_class< BASE, deep >( visitor );
			}
		});
	}
	template< typename CLASSES, bool deep = true >
	constexpr void visit_classes( auto visitor )
	{
		CLASSES::for_each( [ & ]< typename CLASS >()
		{ 
			visit_class< CLASS, deep >( visitor );
		});
	}

	using bases_t =	std::vector< const std::type_info* >;
	struct class_with_bases
	{
		const std::type_info* self;
		v_table* v_table;
		bases_t bases;
	};
	using classes_with_bases = std::map< std::type_index, class_with_bases >;

	constexpr auto declare_visitor( classes_with_bases& registry )
	{
		return overload
			{ [&]< typename C >				
				{ 
					registry[ typeid( C ) ].self = &typeid( C );  
					registry[ typeid( C ) ].v_table = v_table_of< C >();  
				}
			, [&]< typename C, typename B >	{ registry[ typeid( C ) ].bases.emplace_back( &typeid( B ) ); }
			};	
	}
	template< typename CLASS, bool deep = true >
	constexpr void declare( classes_with_bases& registry )
	{
		class_hierarchy::visit_class< CLASS, deep >( declare_visitor( registry ) );
	}
	template< typename CLASSES, bool deep = true >
	constexpr void declare_all( classes_with_bases& registry )
	{
		class_hierarchy::visit_classes< CLASSES, deep >( declare_visitor( registry ) );
	}
	template< typename CLASS >
	constexpr void declare_deep( classes_with_bases& registry )
	{
		declare< CLASS, true >( registry );
	}
	template< typename CLASS >
	constexpr void declare_shallow( classes_with_bases& registry )
	{
		declare< CLASS, false >( registry );
	}

	constexpr void visit_hierarchy( const std::type_index& self, const classes_with_bases& classes_with_bases, auto visitor );
	inline constexpr void visit_bases( const bases_t& bases, const classes_with_bases& classes_with_bases, auto visitor )
	{
		for( auto base : bases )
			visit_hierarchy( *base, classes_with_bases, visitor );
	}
	inline constexpr void visit_hierarchy( const std::type_index& self, const classes_with_bases& classes_with_bases, auto visitor )
	{
		auto found = classes_with_bases.find( self );
		if( found == classes_with_bases.end() )
			throw std::runtime_error("class not registered.");
		visitor( *found->second.self );
		visit_bases( found->second.bases, classes_with_bases, visitor );
	}
}

struct domain
{
	class_hierarchy::classes_with_bases		classes;
	std::vector< type_info_dispatch* >		method_dispatches;	
};

class error;

class v_table
{
public:
	constexpr v_table( const std::type_info& type_info )
		: type_info_( type_info )
	{}
	constexpr const std::type_info& type() const { return type_info_; }
	using v_table_target_t = void(*)();
	constexpr void set_method( int method_index, v_table_target_t target )
	{
		ensure_size( method_index + 1  );
		table_[ method_index ] = target;
	}
	template< typename TRAGET >
	constexpr void set_method( int method_index, TRAGET target )
	{
		auto v_table_target = reinterpret_cast< v_table_target_t >( target );
		set_method( method_index, v_table_target );
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
	std::vector< v_table_target_t > table_;
	constexpr void ensure_size( std::size_t s )
	{
		if( table_.size() >= s )
			return;
		table_.insert( table_.end(), s  - table_.size(), nullptr );
	}
};
template< typename CLASS > constexpr v_table* v_table_of()
{
	static v_table v_table_{ typeid( CLASS ) };
	return &v_table_;
}

class error : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

class type_info_dispatch
{
public:
	using dispatch_target_t = void(*)();
private:
	using method_table_t = std::map< std::type_index, dispatch_target_t >;;
	using entry_t = method_table_t::value_type;
	method_table_t dispatchTable_;
	dispatch_target_t default_ = reinterpret_cast< dispatch_target_t >( &throw_not_found );
	const int v_table_index_ = -1;
public:
	type_info_dispatch() = default;
	type_info_dispatch( domain& domain )
		: v_table_index_( (int)domain.method_dispatches.size() )
	{ 
		domain.method_dispatches.push_back( this ); 
	}
	int v_table_index() const { return v_table_index_; }
	static void throw_not_found(){ throw error( "no target specified." ); }
	void define_default( auto f )
	{
		default_ = reinterpret_cast< dispatch_target_t >( f );
	}
	auto get_default() const
	{
		return default_;
	}
	auto override_erased( const std::type_info& register_type_info, auto f )
	{
		auto t = reinterpret_cast< dispatch_target_t >( f );
		if( is_defined( register_type_info ) )
			throw error( "Method for type already registered." );
		dispatchTable_[ register_type_info ] = t;
		return definition{};
	}
	template< typename TARGET = dispatch_target_t >
	TARGET is_defined( const std::type_info& type_info ) const
	{
		auto found = dispatchTable_.find( type_info );
		if( found != dispatchTable_.end() )
			return reinterpret_cast< TARGET >( found->second );
		return nullptr;
	}
	template< typename TARGET = dispatch_target_t >
	TARGET lookup( const std::type_info& type_info ) const
	{
		if( auto found = is_defined( type_info ) )
			return reinterpret_cast< TARGET >( found );
		return reinterpret_cast< TARGET >( default_ );
	}
	struct definition{};
private:
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
	using virtual_void_t = std::pair< const v_table*, dispatch_t >;
	using erased_function_t = R(*)( ARGS... );
private:
	type_info_dispatch methodTable_;
public:
	method( domain& domain )
		: methodTable_( domain )
	{}
	auto override_erased( const std::type_info& ti, erased_function_t f ) { return methodTable_.override_erased( ti, f ); }
	template< typename CLASS, typename FUNCTION >
	auto override_( FUNCTION f )
	{
		auto fp = ensure_function_ptr< CLASS, ARGS... >( f );
		return methodTable_.override_erased( typeid( CLASS ), fp );
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
		const v_table& v_table = *param.first;
		auto erased_function = reinterpret_cast< erased_function_t >( v_table[ methodTable_.v_table_index() ] );
		return (erased_function)( param.second, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename CLASS, typename... OTHER_ARGS >
	R operator()( CLASS* param, OTHER_ARGS&&... args ) const // to simplify tests!
	{
		return (*this)( to_typed_void( param ), std::forward< OTHER_ARGS >( args )... );
	}
	template< typename POINTER, typename... OTHER_ARGS >
	R operator()( const POINTER& pointer, OTHER_ARGS&&... args ) const
		requires VtableDispatchableVoid< POINTER, dispatch_t >
	{
		virtual_void_t param{ pointer.v_table(), pointer.data() }; 
		return (*this)( param, std::forward< OTHER_ARGS >( args )... );
	}
	template< typename POINTER, typename... OTHER_ARGS >
	R call( const POINTER& pointer, OTHER_ARGS&&... args ) const
		requires VtableDispatchableVoid< POINTER, dispatch_t >
	{
		virtual_void_t param{ pointer.v_table(), pointer.data() }; 
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
	type_info_dispatch methodTable_;
public:
	auto override_erased( const std::type_info& ti, factory_function_t f ) { return methodTable_.override_erased( ti, f ); }
	template< typename CLASS, typename FACTORY >
	auto override_( FACTORY f )
	{
		auto fp = ensure_factory_ptr< CLASS >( f );
		return methodTable_.override_erased( typeid( CLASS ), reinterpret_cast< factory_function_t >( fp ) );
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

template< typename CLASS, typename DEFINITION >
void fill_with_overload( DEFINITION& method, const auto& wrapper )
{
	static_assert( !std::is_const_v< DEFINITION > );
	if( !method.is_defined< CLASS >() )
		method.override_< CLASS >( wrapper );
}
template< typename CLASSES, typename DEFINITION >
void fill_with_overloads( DEFINITION& method, const auto& wrapper )
{
	class_hierarchy::visit_classes< CLASSES >( 
		overload
		{ [&]< typename C >				{ fill_with_overload< C >( method, wrapper ); }
		, [&]< typename C, typename B >	{}
		});
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
				method->override_erased( *class_with_bases.self, found );
}
inline void interpolate( const domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		interpolate( domain.classes, method );
}
inline void set_v_table( const class_hierarchy::class_with_bases& class_, const type_info_dispatch& method )
{
	auto target = method.is_defined( *class_.self );
	if( !target )
		target = method.get_default();
	class_.v_table->set_method( method.v_table_index(), target );
}
inline void build_v_tables( const class_hierarchy::classes_with_bases& classes, const type_info_dispatch& method )
{
	for( const auto& class_ : classes )
		set_v_table( class_.second, method );
}
inline void build_v_tables( const domain& domain )
{
	for( const auto& method : domain.method_dispatches )
		build_v_tables( domain.classes, *method );
}
inline void build_runtime( const domain& domain )
{
	interpolate( domain );
	build_v_tables( domain );
}

template< template< typename > typename CONST, typename FOUND, typename FROM > auto typeid_cast_implementation_( auto* from, const std::type_info& to )
{
	typename CONST< void >::type found = nullptr;
	class_hierarchy::visit_class< FROM >
		( overload
		{ [&]< typename C >				
			{ 
				if( !found&& typeid( FROM ) == to ) 
					found = static_cast< CONST< void >::type >( from ); 
			}
		, [&]< typename, typename B >	
			{	
				if( !found ) 
					found = typeid_cast_implementation_< CONST, FOUND, B >( static_cast< CONST< B >::type >( from ), to ); }
		});
	return found;
}
struct typeid_const_cast_implementation
{
	template< typename T > struct const_ {	using type = const T*; }; 
	template< typename FROM > auto operator()( const FROM* from, const std::type_info& to )
	{
		return typeid_cast_implementation_< const_, typed_const_void, FROM >( from, to );
	}
};
struct typeid_cast_implementation
{
	template< typename T > struct non_const_ { using type = T; }; 
	template< typename FROM > auto operator()( FROM* from, const std::type_info& to )
	{
		return typeid_cast_implementation_< non_const_, typed_void, FROM >( from, to );	
	}
};
using typeid_const_cast_method = method< const void*( const void*, const std::type_info& to ) >;
using typeid_cast_method = method< void*( const void*, const std::type_info& to ) >;
template< typename CLASSES >
void fill_const_cast_for( typeid_const_cast_method& method )
{
	fill_with_overloads< CLASSES >( method, typeid_const_cast_implementation{} );
}
template< typename CLASSES >
void fill_cast_for( typeid_cast_method& method )
{
	fill_with_overloads< CLASSES >( method, typeid_cast_implementation{} );
}

}