#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/lifetime/cast.h"

using namespace Catch::Matchers;

using namespace virtual_void;

const double M_PI = 3.14;

struct position {float x, y;};

struct X 
{ 
    std::string s_;
    std::string to_string() const { return s_; } 
};

ERASED_INTERFACE( to_string_i,
    (std::string, to_string)
)

using to_string_sc = to_string_i< erased::shared_const  >;
using to_string_co = to_string_i< erased::const_observer >;

namespace virtual_void::erased
{
    template< typename ERASED_TO >
    auto v_table_cast( const auto& v_table )
    {
        return reinterpret_cast< typename ERASED_TO::_v_table_t* >( v_table ); 
    }

    template< typename ERASED_TO, typename ERASED_FROM >
    ERASED_TO interface_lifetime_cast( const ERASED_FROM& from )
    {
        return ERASED_TO
            ( lifetime_cast< typename ERASED_TO::erased_t >( from._ref )
            , v_table_cast< ERASED_TO >( from._v_table )
            );
    }
}

TEST_CASE( "interface lifetime cast" ) 
{
    to_string_sc sc{ X{ "hallo" } };
    REQUIRE( sc.to_string() == "hallo" );

    to_string_co co = interface_lifetime_cast< to_string_co >( sc );
    REQUIRE( co.to_string() == "hallo" );
}