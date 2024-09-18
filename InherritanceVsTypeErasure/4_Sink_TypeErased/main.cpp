#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <map>
#include <vector>
#include <utility>

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
}

int main()
{
    using namespace Application;
    DB::System db;
    db.factories[ "i" ] = []( const std::string& data )->std::any{  return IntData{std::atoi( data.c_str() ) }; };
    db.factories[ "s" ] = []( const std::string& data )->std::any{  return StringData{ data }; };
    db.Query( "junk", []( const std::any& any ){
        if( auto i = std::any_cast< IntData >( &any ) )
            std::cout << "int: " << i->data << std::endl;
        else if( auto s = std::any_cast< StringData >( &any ) )
            std::cout << "string: " << s->data << std::endl;

    });
    return 0;
}