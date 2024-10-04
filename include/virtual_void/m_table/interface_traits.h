#pragma once

#include "../erased/generic_traits.h"
#include "lifetime.h"

namespace virtual_void::erased
{
    struct m_table_make_shared
    {
        template< typename FROM > auto operator()( FROM&& from )
        {
            return 	virtual_void::m_table::make_shared_const< std::remove_cvref_t< FROM > >( std::forward< FROM >( from ) );
        }
    };

    template<> struct trait< virtual_void::m_table::shared_const > 
        : erased::erase_trait
            < virtual_void::m_table::shared_const 
            , m_table_make_shared
            >{};

};
