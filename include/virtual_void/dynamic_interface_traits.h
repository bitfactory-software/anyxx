#pragma once

#include "erased/interface.h"
#include "virtual_void.h"

namespace virtual_void::erased
{
    template<>
    struct trait< shared_const >
    {
        using type = shared_const;
        
        using param_t = const type&;

        template< typename FROM >
        static type erase( FROM&& from )
        {
            using from_t = std::remove_cvref_t< FROM >;
            if constexpr( std::is_base_of_v< type, from_t > )
            {
                return from;
            }
            else
            {
                return 	virtual_void::make_shared_const< from_t >( std::forward< FROM >( from ) );
            }
        }
        template< typename CONSTRUCTOR_PARAM >
        static auto unerase( const type& from )
        {
            using constructor_param_t = std::remove_cvref_t< CONSTRUCTOR_PARAM >;
            if constexpr( std::is_base_of_v< type, constructor_param_t > )
            {
                return static_cast< const constructor_param_t::wrapped_type * >( from.data() );
            }
            else
            {
                return static_cast< const constructor_param_t* >( from.data() );
            }
        }
    };

};
