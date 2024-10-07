#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/lifetime/observer.h"
#include "../include/virtual_void/erased/lifetime/shared_const.h"

using namespace::virtual_void;

ERASED_FREE_INTERFACE(to_string_i,
    (std::string, to_string)
)

using to_string_vv = to_string_i< erased::shared_const >;
using to_string_co = to_string_i< erased::const_observer >;

std::string to_string_( auto const x )
{
    return std::to_string( x.count_sides() );
}

//namespace virtual_void::erased
//{
//    template< typename TO, typename ERASED_INTERFACE >
//    TO interface_lifetime_cast( const ERASED_INTERFACE& interface )
//    {
//        auto casted = lifetime_cast< typename ERASED_INTERFACE::erased_t >( interface.get_erased() );
//        return { casted, interface._v_table }; 
//    }
//
//    //template< typename TO, typename ERASED_INTERFACE >
//    //TO interface_lifetime_move_cast( )
//    //{
//    //
//    //}
//
//}
//
//TEST_CASE( "erased interface change erased" ) 
//{
//    to_string_vv vv_{ 4711 };
//    to_string_co co_{ vv_, erased::reconcrete_cast< int >( vv_.get_erased() ) }
//
//}
