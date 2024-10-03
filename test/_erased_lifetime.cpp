#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/lifetime.h"

using namespace Catch::Matchers;

using namespace virtual_void;

namespace
{
    TEST_CASE( "erased/lifetime" )
    {
        {
		    auto u1 = std::make_unique< erased::concrete_data< int > >( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( *u1 ) == 1 );
        }
        {
		    const auto u1 = std::make_unique< erased::concrete_data< int > >( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( *u1 ) == 1 );
        }
        {
		    auto u1 = erased::unique( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( u1 ) == 1 );
        }
        {
		    const auto u1 = erased::unique( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( u1 ) == 1 );
        }
        {
           auto t1 = erased::typed_unique( 1 );
           *t1 = 2;
           REQUIRE( *t1 == 2 );
           //auto e1 = t1; // shall not compile!
           auto e1 = std::move( t1 );
           REQUIRE( !t1 ); // moved
           REQUIRE( e1.data() );
           t1 = as< int >( std::move( e1 ) );
           REQUIRE( !e1 ); // moved
           REQUIRE( *t1 == 2 );
        }
    }
}
