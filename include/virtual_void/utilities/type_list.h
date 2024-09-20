#pragma once

namespace virtual_void
{
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