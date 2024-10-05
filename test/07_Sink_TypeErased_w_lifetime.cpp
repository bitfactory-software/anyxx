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
#include "../../include/virtual_void/m_table/lifetime.h"
#include "../../include/virtual_void/typeid/cast.h"
#include "../../include/virtual_void/open_method/algorithm.h"

namespace
{

namespace DB
{
    using FactoryFunction = std::function< virtual_void::m_table::shared_const( const std::string& ) >;
    using SinkFunction = std::function< void( const virtual_void::m_table::shared_const& ) >;

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
            ,   { "ss", "chief" }
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
        StringData( const std::string& s1 ) 
            : data( s1 )
        {}
        std::string data;
    }; 
    struct SuperStringData : StringData
    {
        SuperStringData( const std::string& s1, const std::string& s2 ) 
            : StringData( s1 )
            , more( s2 )
        {}
        std::string more;
    }; 
    struct DoubleData
    {
        double data;
    }; 

    std::string ToString_( const auto* x )
    {
        return std::to_string( x->data );
    }

    std::string ToString_( const StringData* x )
    {
        return x->data;
    }

    std::string ToString_( const SuperStringData* x )
    {
        return x->data + " -> " + x->more;
    }

    virtual_void::domain applicationDomain;

    auto entityToOut = virtual_void::method< void( const void* ) >{ applicationDomain };
    auto toString = virtual_void::method< std::string( const void* ) >{ applicationDomain };
    auto typeid_const_cast = virtual_void::typeid_::const_cast_method{ applicationDomain };

    void IntToOut( const IntData* i ){ std::cout << "int: " << i->data << std::endl; }

    void AnywhereInTheApplication()
    {
        entityToOut.define< IntData >( &IntToOut );
    }
}

}
namespace virtual_void::class_hierarchy
{
	using namespace Application;
	template<> struct class_< StringData >: base {};
	template<> struct class_< SuperStringData >: bases< StringData >{};
	template<> struct class_< IntData >: base {};
	template<> struct class_< DoubleData >: base {};
}

namespace
{

namespace Application
{
    using classes = virtual_void::type_list< SuperStringData, StringData, IntData, DoubleData >;
}


TEST_CASE( "07_Sink_TypeErased_w_lifetime" ) 
{
    using namespace Application;
    using namespace DB;

    virtual_void::open_method::declare_classes( classes{}, applicationDomain );
 
    AnywhereInTheApplication();

    DB::System db;

    entityToOut.define< StringData >( []( const StringData* s ) 
        { 
            std::cout << "string: " << s->data << std::endl; 
        });
 
    virtual_void::open_method::fill_with_overloads( classes{}, toString, []( const auto* x ){ return ToString_( x ); } );
    virtual_void::typeid_::fill_const_cast_for( classes{}, typeid_const_cast );

    virtual_void::open_method::build_m_tables( applicationDomain );

    db.factories[ "i" ] = []( const std::string& data ){  return virtual_void::m_table::make_shared_const< IntData >( std::atoi( data.c_str() ) ); };
    db.factories[ "s" ] = []( const std::string& data ){  return virtual_void::m_table::make_shared_const< StringData >( data ); };
    db.factories[ "ss" ] = []( const std::string& data ){  return virtual_void::m_table::make_shared_const< SuperStringData >( data, "boss" ); };
    db.factories[ "d" ] = []( const std::string& data ){  return virtual_void::m_table::make_shared_const< DoubleData >( std::atof( data.c_str() ) ); };

    db.Query( "junk", []( const virtual_void::m_table::shared_const& e )
    { 
        // call open method
        std::cout << "type_info: " << e.type().name() << ": " << toString( e ) << std::endl;
        
        try
        {
            // call open method
            entityToOut( e );
        }
        catch( std::exception& e )
        {
            std::cout << "error: " << e.what() << std::endl;
        }

        // cast back from erased -> "unerase"
        if( auto stringData = virtual_void::typeid_::cast_to< const StringData >( typeid_const_cast, e ) )
            std::cout << "stringData: " << stringData->data << std::endl;
    });

 
}

}