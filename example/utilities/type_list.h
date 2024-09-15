#pragma once

namespace BitFactory
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
}