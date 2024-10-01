#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <typeindex>
#include <memory>

#include "include/catch.hpp"

#include "../../include/std26/proxy.h"
#include "../../include/virtual_void/virtual_void.h"
#include "../../include/virtual_void/proxy/adapter.h"

namespace
{

namespace DB
{
    PRO_DEF_FREE_DISPATCH(FreeToString, ToString_, ToString);

    struct EntityFacade : pro::facade_builder
        ::add_facade< virtual_void::proxy::virtual_void_facade, true >
//        ::add_facade< virtual_void::proxy::m_table_facade, true >
        ::add_convention<FreeToString, std::string() const>
        ::support_copy<pro::constraint_level::nontrivial>
        ::build {};


    using Entity = pro::proxy< EntityFacade >;

    using FactoryFunction = std::function< Entity( const std::string& ) >;
    using SinkFunction = std::function< void( const Entity& ) >;

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
    using virtual_void::proxy::data_;
   // using virtual_void::proxy::m_table_of;

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

    std::string ToString_( const auto& x )
    {
        return std::to_string( x.data );
    }

    std::string ToString_( const StringData& x )
    {
        return x.data;
    }

    virtual_void::domain applicationDomain;

    auto entityToOut = virtual_void::method< void( const void* ) >{ applicationDomain };

    void IntToOut( const IntData* i ){ std::cout << "int: " << i->data << std::endl; }

    void AnywhereInTheApplication()
    {
        entityToOut.override_< IntData >( &IntToOut );
    }
}

TEST_CASE( "06_Sink_TypeErased_w_proxy" ) 
{
    using namespace Application;
    using namespace DB;

    AnywhereInTheApplication();

    DB::System db;

    entityToOut.override_< StringData >( []( const StringData* s ) 
        { 
            std::cout << "string: " << s->data << std::endl; 
        });

    entityToOut.seal();

    db.factories[ "i" ] = []( const std::string& data )->DB::Entity{  return std::make_shared< IntData >( std::atoi( data.c_str() ) ); };
    db.factories[ "s" ] = []( const std::string& data )->DB::Entity{  return std::make_shared< StringData >( data ); };
    db.factories[ "d" ] = []( const std::string& data )->DB::Entity{  return std::make_shared< DoubleData >( std::atof( data.c_str() ) ); };
    try
    {
        db.Query( "junk", []( const DB::Entity& e )
            { 

                std::cout << "type_info: " << pro::proxy_reflect< virtual_void::proxy::meta >( e ).type_info.name() << ": " << ToString( *e )<< std::endl;
                entityToOut( virtual_void::proxy::to_typeid_const_void( e ) );    
            });
    }
    catch( std::exception& e )
    {
        std::cout << "error: " << e.what() << std::endl;
    }
 
}

}