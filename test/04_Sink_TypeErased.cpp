#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <map>
#include <vector>
#include <utility>

#include "include/catch.hpp"

namespace
{

namespace DB
{
    using FactoryFunction = std::function< std::any( const std::string& ) >;
    using SinkFunction = std::function< void( const std::any& ) >;
    using Line = std::pair< std::string, std::string >;
    using File = std::vector< Line >;
    struct System
    {
        File file;
        std::map< std::string, FactoryFunction > factories;
        void Query( const SinkFunction& sink )
        {
            for( auto [ type, data ] : file )
                sink( factories[ type ]( data ) );
        }        
    };
}

namespace Application
{
    struct IntData
    {
        int data = 0;
        std::string ToString() const { return "Int: " + std::to_string( data ); }
    }; 
    struct StringData
    {
        std::string data;
        std::string ToString() const { return "String: " + data; }
    }; 
    void ReportSink( const std::any& any )
    {
        if( auto i = std::any_cast< IntData >( &any ) )
            std::cout << "int: " << i->ToString() << std::endl;
        else if( auto s = std::any_cast< StringData >( &any ) )
            std::cout << "string: " << s->ToString() << std::endl;
    }
}

TEST_CASE( "04_Sink_TypeErased" ) 
{
    using namespace Application;
    DB::System db;
    db.file =
    {   { "i", "1" }
    ,   { "i", "4711" }
    ,   { "s", "hello" }
    ,   { "s", "world" }
    };
    db.factories[ "i" ] = []( const std::string& data )->std::any{  return IntData{std::atoi( data.c_str() ) }; };
    db.factories[ "s" ] = []( const std::string& data )->std::any{  return StringData{ data }; };
    db.Query( ReportSink );
 
}

}