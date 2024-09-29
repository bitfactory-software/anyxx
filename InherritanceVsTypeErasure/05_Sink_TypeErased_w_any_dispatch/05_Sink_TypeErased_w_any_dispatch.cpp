#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <typeindex>

#include "../../include/any_dispatch/method.h"

namespace DB
{
    using FactoryFunction = std::function< std::any( const std::string& ) >;
    using SinkFunction = std::function< void( const std::any& ) >;

    struct System
    {
        std::map< std::string, FactoryFunction > factories;
        void Query( std::string what, const SinkFunction& sink )
        {
            auto dataLines = std::vector< std::pair< std::string, std::string > >  
            {   { "i", "1" }
            ,   { "i", "4711" }
            ,   { "s", "hello" }
            ,   { "s", "world" }
            ,   { "d", "3.14" }
            };
            for( auto [ type, data ] : dataLines )
                sink( factories[ type ]( data ) );
        }        
    };
}

namespace Application
{
    struct IntData
    {
        int data = 0;
    }; 
    struct StringData
    {
        std::string data;
    }; 
    struct DoubleData
    {
        double data;
    }; 

    auto AnyToOut = any_dispatch::method< void, const std::any& >{};

    void IntToOut( const IntData* i ){ std::cout << "int: " << i->data << std::endl; }

    void AnywhereInTheApplication()
    {
        AnyToOut.define< const IntData >( &IntToOut );
    }
}

int main()
{
    using namespace Application;

    AnywhereInTheApplication();

    DB::System db;

    AnyToOut.define< const StringData >( []( const auto s )
        { std::cout << "string: " << s->data << std::endl; } );

    db.factories[ "i" ] = []( const std::string& data )->std::any{  return IntData{std::atoi( data.c_str() ) }; };
    db.factories[ "s" ] = []( const std::string& data )->std::any{  return StringData{ data }; };
    db.factories[ "d" ] = []( const std::string& data )->std::any{  return DoubleData{ std::atof( data.c_str() ) }; };
    try
    {
        db.Query( "junk", AnyToOut );
    }
    catch( std::exception& e )
    {
        std::cout << "error: " << e.what() << std::endl;
    }
    return 0;
}