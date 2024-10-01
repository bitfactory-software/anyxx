#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/dynamic_interface/dynamic_interface.h"
#include "../include/virtual_void/dynamic_interface_traits.h"

using namespace Catch::Matchers;

template< typename T >
struct trait;

template<>
struct trait< const void * >
{
    static const bool is_const = true;
};

template<>
struct trait< void * >
{
    static const bool is_const = false;
};

template< typename VOID >
struct test_interface
{
    VOID member;


    std::string f( int ) requires ( !trait<VOID>::is_const )
    {
        return "mutable"; 
    }

    std::string f( int ) const 
    { 
      return "const"; 
    }
};


TEST_CASE( "_interface_const_correct.cpp" ) {

    static_assert( !trait<void* >::is_const );
    static_assert( trait<const void* >::is_const );
    test_interface< const void* > i1;
    REQUIRE( i1.f( 1 ) == "const");

    test_interface< const void* > const i2;
    REQUIRE( i2.f( 1 ) == "const");

    test_interface< void* > i3;
    REQUIRE( i3.f( 1 ) == "mutable");

    test_interface< void* > const i4;
    REQUIRE( i4.f( 1 ) == "const");
}

