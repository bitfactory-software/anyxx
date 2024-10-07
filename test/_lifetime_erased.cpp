#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/catch.hpp"

#include "../include/virtual_void/erased/lifetime/data.h"
#include "../include/virtual_void/erased/lifetime/observer.h"
#include "../include/virtual_void/erased/lifetime/shared_const.h"
#include "../include/virtual_void/erased/lifetime/unique.h"
#include "../include/virtual_void/erased/lifetime/value.h"

using namespace Catch::Matchers;

using namespace virtual_void;

namespace
{
    TEST_CASE( "erased/lifetime/data" )
    {
        {
		    auto u1 = std::make_unique< erased::concrete_data< int > >( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( *u1 ) == 1 );
        }
        {
		    const auto u1 = std::make_unique< erased::concrete_data< int > >( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( *u1 ) == 1 );
        }
    }
    TEST_CASE( "erased/lifetime/observer" )
    {
        {
            static_assert( std::same_as< erased::select_observer< std::string >::type, erased::mutable_observer > );
            static_assert( std::same_as< erased::select_observer< const std::string >::type, erased::const_observer > );

            std::string s{ "hallo"};
		    auto mo = erased::mutable_observer( s );
            static_assert( std::same_as< erased::typed_observer< std::string >::conrete_t, std::string > );
            static_assert( std::same_as< erased::typed_observer< std::string const >::conrete_t, std::string const > );
		    auto co = erased::const_observer( s );
            auto tmo = erased::typed_observer< std::string >( mo );
            static_assert( std::derived_from< decltype(tmo), erased::mutable_observer > );
            *tmo = "world";
            REQUIRE( s == "world" );
            REQUIRE( *reconcrete_cast< const std::string >( co ) == "world" );
            REQUIRE( *reconcrete_cast< std::string >( mo ) == "world" );
            // auto tmo2 = erased::typed_observer< std::string * >{ co }; // shall not compile
        }
        {
            const std::string s{ "hallo"};
		    // auto mo = erased::mutable_observer( &s ); // shall not compile
		    auto co = erased::const_observer( s );
            auto tco = erased::typed_observer< std::string const >( co );
            static_assert( std::derived_from< decltype(tco), erased::const_observer > );
            REQUIRE( *tco == "hallo" );
            REQUIRE( *reconcrete_cast< const std::string >( co ) == "hallo" );
        }
    }
    TEST_CASE( "erased/lifetime/unique" )
    {
        {
		    auto u1 = erased::unique( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( u1 ) == 1 );
        }
        {
		    const auto u1 = erased::unique( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( u1 ) == 1 );
        }
        {
            struct A { std::string s; };            
            auto u1 = erased::unique( std::in_place_type< A >, "hallo" );
            REQUIRE( *erased::reconcrete_cast< std::string >( u1 ) == "hallo" );
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
    TEST_CASE( "erased/lifetime/shared_const" )
    {
        {
		    auto u1 = erased::shared_const( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( u1 ) == 1 );
        }
        {
		    const auto u1 = erased::shared_const( 1 );
            REQUIRE( *erased::reconcrete_cast< int >( u1 ) == 1 );
        }
        {
            struct A { std::string s; };            
            auto u1 = erased::shared_const( std::in_place_type< A >, "hallo" );
            REQUIRE( *erased::reconcrete_cast< std::string >( u1 ) == "hallo" );
        }
        {
            auto t1 = erased::typed_shared_const< int >( 1 );
            //*t1 = 2; // shall not compile!
            REQUIRE( *t1 == 1 );
            //auto e1 = t1; // shall not compile!
            auto e1 = t1;
            REQUIRE( t1 ); // !moved
            REQUIRE( e1.data() );
            t1 = as< int >( std::move( e1 ) );
            REQUIRE( !e1 ); // !moved
            REQUIRE( *t1 == 1 );
        }
    }
    TEST_CASE( "erased/lifetime/value" )
    {
        using namespace erased;
        {
		    auto u1 = value( 1 );
            REQUIRE( *reconcrete_cast< int >( u1 ) == 1 );
        }
        {
            struct A { std::string s; };            
            auto u1 = value( std::in_place_type< A >, "hallo" );
            REQUIRE( reconcrete_cast< A >( u1 )->s == "hallo" );
            auto u2 = u1;
            REQUIRE( reconcrete_cast< A >( u1 )->s.data() != reconcrete_cast< A >( u2 )->s.data() );
            REQUIRE( reconcrete_cast< A >( u2 )->s == "hallo" );
            reconcrete_cast< A >( u2 )->s = "world";
            REQUIRE( reconcrete_cast< A >( u1 )->s == "hallo" );
            REQUIRE( reconcrete_cast< A >( u2 )->s == "world" );
        }
        {
            auto t1 = typed_value< int >( 1 );
            *t1 = 2;
            REQUIRE( *t1 == 2 );
            auto e1 = t1;
            REQUIRE( t1 ); // !moved
            REQUIRE( e1.data() );
            t1 = as< int >( std::move( e1 ) );
            REQUIRE( !e1 ); // !moved
            REQUIRE( *t1 == 2 );
        }
        {
            auto t1 = make_value< std::string >( "hallo" );
            REQUIRE( *t1 == "hallo" );
        }
        {
            std::string a = "hallo";
            auto t1 = make_value_t{}( a );
            REQUIRE( *reconcrete_cast< std::string >( t1 )== "hallo" );
        }
        {
            struct x_t
            {
                std::string s_;
            };
            x_t a{ "hallo" };
            auto t1 = make_value_t{}( a );
            REQUIRE( reconcrete_cast< x_t >( t1 )->s_ == "hallo" );
        }
    }
    TEST_CASE( "erased/lifetime/convert" )
    {
    }
}
