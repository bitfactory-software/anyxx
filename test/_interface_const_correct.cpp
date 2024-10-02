#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/lifetime.h"
#include "../include/virtual_void/erased/lifetime_traits.h"
#include "../include/virtual_void/m_table/dynamic_interface_traits.h"

using namespace Catch::Matchers;

namespace
{
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
        VOID member = nullptr;
        std::string f( int ) requires ( !trait<VOID>::is_const ) { return "mutable"; }
        std::string f( int ) const { return "const"; }
    };
    TEST_CASE( "_interface_const_correct prototyping" )
    {
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
}

namespace
{
    struct functor
    {
        std::string text = "hallo";
        std::string operator()() const { return text; }
        void operator()( const std::string&  t ) { text = t; }
    };

    TEST_CASE( "_interface_const_correct void (const) *" ) 
    {
        using const_function    = virtual_void::erased::call_operator_facade< const void*, virtual_void::erased::base, std::string() >;
        using mutating_function = virtual_void::erased::call_operator_facade< void*,       virtual_void::erased::base, void(std::string) >;


        {
            functor function_object;
            const_function cf = function_object;
            mutating_function mf = function_object;
            REQUIRE( cf() == "hallo" );
            mf( "world");
            REQUIRE( cf() == "world" );
        }

        {
            functor function_object;
            const_function const cf = function_object;
            mutating_function const mf = function_object;
            REQUIRE( cf() == "hallo" );
            mf( "world");
            REQUIRE( cf() == "world" );
        }

        {
            functor const const_function_object;
            const_function cf = const_function_object;
            //mutating_function mf = const_function_object; // <- may not compile!
            REQUIRE( cf() == "hallo" );
        }

        {
            functor const function_object;
            const_function const cf = function_object;
            // mutating_function const mf = function_object; // <- may not compile!
            REQUIRE( cf() == "hallo" );
        }

    }

    TEST_CASE( "_interface_const_correct virtual_void::shared_const" ) 
    {
        using const_function    = virtual_void::erased::call_operator_facade< virtual_void::m_table::shared_const, virtual_void::erased::base, std::string() >;
        using mutating_function = virtual_void::erased::call_operator_facade< virtual_void::m_table::shared_const, virtual_void::erased::base, void(std::string) >;


        {
            functor function_object;
            const_function cf = function_object;
            //mutating_function mf = function_object; //  <- may not compile!
            REQUIRE( cf() == "hallo" );
        }

        {
            functor function_object;
            const_function const cf = function_object;
            //mutating_function const mf = function_object;  // <- may not compile!
            REQUIRE( cf() == "hallo" );
        }

        {
            functor const const_function_object;
            const_function cf = const_function_object;
            //mutating_function mf = const_function_object; // <- may not compile!
            REQUIRE( cf() == "hallo" );
        }

        {
            functor const function_object;
            const_function const cf = function_object;
            //mutating_function const mf = function_object; // <- may not compile!
            REQUIRE( cf() == "hallo" );
        }
    }
}
