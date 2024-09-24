#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

namespace DB
{
    struct IAny
    {
        virtual ~IAny() = default;   
    }; 
    using FactoryFunction = std::function< IAny*( const std::string& ) >;
    using SinkFunction = std::function< void( const IAny* ) >;
    using Line = std::pair< std::string, std::string >;
    using File = std::vector< Line >;
    struct System
    {
        File file;
        std::map< std::string, FactoryFunction > factories;
        void Query( const SinkFunction& sink )
        {
            for( auto [ type, data ] : file )
                if( std::unique_ptr< IAny > any{ factories[ type ]( data ) } )
                    sink( any.get() );
        }        
    };
}

namespace Application
{
    struct Data : DB::IAny
    {
        virtual std::string ToString() const = 0;
    }; 
    struct  IntData final : Data
    {
        int data = 0;
        IntData( int i ) : data( i ) {}
        std::string ToString() const override { return "Int: " + std::to_string( data ); }
    }; 
    struct StringData final : Data
    {
        std::string data;
        StringData( const std::string& s ) : data( s ) {}
        std::string ToString() const override { return "String: " + data; }
    }; 
    void ReportSink( const DB::IAny* any )
    {
        if( auto data = dynamic_cast< const Data* >( any ) )
            std::cout << data->ToString() << std::endl;
    }
}

int main()
{
    using namespace Application;
    DB::System db;
    db.file =
    {   { "i", "1" }
    ,   { "i", "4711" }
    ,   { "s", "hello" }
    ,   { "s", "world" }
    };
    db.factories[ "i" ] = []( const std::string& data ){ return new IntData{ std::atoi( data.c_str() ) }; };
    db.factories[ "s" ] = []( const std::string& data ){ return new StringData{ data }; };
    db.Query( ReportSink );
    return 0;
}