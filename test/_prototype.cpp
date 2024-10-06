#include <iostream>

#include "include/catch.hpp"

namespace
{

template< typename T > void trace_alignof()
{
    std::cout << "alignof(" << typeid( T ).name() << "): " << alignof( T ) << std::endl;

}

TEST_CASE( "prototype" ) 
{
    trace_alignof< const char* >();
    trace_alignof< std::string >();
    trace_alignof< int >();
    trace_alignof< double >();
}

}