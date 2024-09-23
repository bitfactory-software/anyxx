#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace DB
{
    struct IAny
    {
        virtual ~IAny() = default;   
    }; 
    struct IFactory
    {
        virtual IAny* Construct( const std::string& ) const = 0; 
        virtual ~IFactory() = default;   
    }; 
    struct IQueryResultSink
    {
        virtual void Recieve( const IAny* ) const = 0;
    };
    struct System
    {
        std::map< std::string, std::unique_ptr< IFactory > > factories;
        void Query( std::string what, const IQueryResultSink& sink )
        {
            auto dataLines = std::vector< std::pair< std::string, std::string > >  
            {   { "i", "1" }
            ,   { "i", "4711" }
            ,   { "s", "hello" }
            ,   { "s", "world" }
            };
            for( auto [ type, data ] : dataLines )
                if( std::unique_ptr< IAny > any{ factories[ type ]->Construct( data ) } )
                    sink.Recieve( any.get() );
        }        
    };
}


namespace Application
{
    struct Data : DB::IAny
    {
        virtual std::string ToString() const = 0;
    }; 
    struct IntData : Data
    {
        int data = 0;
        IntData( int i ) : data( i ) {}
        std::string ToString() const override { return "Int: " + std::to_string( data ); }
        struct Factory : DB::IFactory
        {
            DB::IAny* Construct( const std::string& data ) const override { return new IntData( std::atoi( data.c_str() ) ); }
        };
    }; 
    struct StringData : Data
    {
        std::string data;
        StringData( const std::string& s ) : data( s ) {}
        std::string ToString() const override { return "String: " + data; }
        struct Factory : DB::IFactory
        {
            DB::IAny* Construct( const std::string& data ) const override { return new StringData( data ); }
        };
    }; 
    struct QueryResultSink : DB::IQueryResultSink
    {
        void Recieve( const DB::IAny* any ) const override
        {
            if( auto data = dynamic_cast< const Data* >( any ) )
                std::cout << data->ToString() << std::endl;
        }
    }; 
}


int main()
{
    using namespace Application;
    DB::System db;
    db.factories[ "i" ] = std::make_unique< IntData::Factory >();
    db.factories[ "s" ] = std::make_unique< StringData::Factory >();
    QueryResultSink sink;
    db.Query( "junk", sink );
    return 0;
}