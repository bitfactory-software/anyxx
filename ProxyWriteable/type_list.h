#pragma once

namespace BitFactory
{
    template< typename... ARGS >
    struct type_list
    {
        static void for_each( auto&& call )
        {
            ( call.template operator()< ARGS >(), ... );
        }
    };
}