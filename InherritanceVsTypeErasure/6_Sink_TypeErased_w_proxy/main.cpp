#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <typeindex>
#include <memory>

#include "../../include/proxy/proxy.h"
#include "../../include/virtual_void/forward.h"
#include "../../include/virtual_void/proxy/adapter.h"

namespace DB
{
    struct meta
    {
        template <class O> constexpr explicit meta( std::in_place_type_t< O > ) 
            : type_info( typeid( O ) ) {}
        template <class O> constexpr explicit meta( std::in_place_type_t< std::shared_ptr< O > > )
            : type_info( typeid( O ) ) {}
        template <class O> constexpr explicit meta( std::in_place_type_t< std::unique_ptr< O > > )
            : type_info( typeid( O ) ) {}
        template <class O> constexpr explicit meta( std::in_place_type_t< O* > )
            : type_info( typeid( O ) ) {}

        //template <class O> constexpr explicit meta( std::in_place_type_t< O > ) 
        //    : type_info( typeid( O ) ) , v_table( v_table_of< O >() ) {}
        //template <class O> constexpr explicit meta( std::in_place_type_t< std::shared_ptr< O > > )
        //    : type_info( typeid( O ) ) , v_table( v_table_of< O >() ) {}
        //template <class O> constexpr explicit meta( std::in_place_type_t< std::unique_ptr< O > > )
        //    : type_info( typeid( O ) ) , v_table( v_table_of< O >() ) {}
        //template <class O> constexpr explicit meta( std::in_place_type_t< O* > )
        //    : type_info( typeid( O ) ) , v_table( v_table_of< O >() ) {}

        const std::type_info& type_info;
        //const v_table*        v_table;
    };

    PRO_DEF_FREE_DISPATCH( free_data, data_, data );

    template< typename O > const void* data_( const O& o ) { return &o; }
    template< typename O > void* data_( O& o ) { return &o; }

    template< typename O > const void* data_( std::shared_ptr< const O >& o ) { return o.get(); }
    template< typename O > void* data_( std::shared_ptr< O >& o ) { return o.get(); }
    template< typename O > const void* data_( std::unique_ptr< const O >& o ) { return o.get(); }

    struct meta_facade : pro::facade_builder
        ::support_copy<pro::constraint_level::nontrivial>
        ::add_reflection< meta >
        ::build {};

    //struct virtual_const_void_facade : pro::facade_builder
    //    ::support_copy<pro::constraint_level::nontrivial>
    //    ::add_facade< meta_facade, true >
    //    ::add_convention< free_data, const void*() const >
    //    ::build {};

    struct virtual_void_facade : pro::facade_builder
        ::support_copy<pro::constraint_level::nontrivial>
        ::add_facade< meta_facade, true >
//        ::add_facade< virtual_const_void_facade, true >
        ::add_convention< free_data, void*() >
        ::build {};

    struct EntityFacade : pro::facade_builder
        ::add_facade< virtual_void_facade, true >
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
    template< typename O > auto data_( O&& o ) { return DB::data_( std::forward< O >( o ) ); }

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

    //auto AnyToOut = library::declare_any_dispatch< void, const std::any& >{};

    void IntToOut( const IntData* i ){ std::cout << "int: " << i->data << std::endl; }

    //void AnywhereInTheApplication()
    //{
    //    AnyToOut.define< const IntData >( &IntToOut );
    //}
}

int main()
{
    using namespace Application;
    using namespace DB;

    //AnywhereInTheApplication();

    DB::System db;

    //AnyToOut.define< const StringData >( []( const auto s )
    //    { std::cout << "string: " << s->data << std::endl; } );

    //using DB::data_;
    //std::string data = "1";
    //auto s = std::make_shared< IntData >( std::atoi( data.c_str() ) );
    //DB::Entity entity( s );
     
    db.factories[ "i" ] = []( const std::string& data )->DB::Entity{  return std::make_shared< IntData >( std::atoi( data.c_str() ) ); };
    db.factories[ "s" ] = []( const std::string& data )->DB::Entity{  return std::make_shared< StringData >( data ); };
    db.factories[ "d" ] = []( const std::string& data )->DB::Entity{  return std::make_shared< DoubleData >( std::atof( data.c_str() ) ); };
    try
    {
        db.Query( "junk", []( const DB::Entity& e )
            { 
                std::cout << "type_info: " << pro::proxy_reflect< DB::meta >( e ).type_info.name() << std::endl;
            });
    }
    catch( std::exception& e )
    {
        std::cout << "error: " << e.what() << std::endl;
    }
    return 0;
}