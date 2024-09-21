#pragma once

#include <typeinfo>
#include <typeindex>

namespace virtual_void
{
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

	class error;

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
}