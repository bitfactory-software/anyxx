#pragma once

#include "../erased/generic_traits.h"
#include "lifetime.h"

namespace virtual_void::erased
{
    template<> struct trait< virtual_void::m_table::shared_const > 
        : erased::erase_trait
            < virtual_void::m_table::shared_const 
            >{};

};
