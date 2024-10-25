#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "include/catch.hpp"

#include "../include/std26/proxy.h"

#include "../include/virtual_void/erased/dispatch/open_method/typeid/open_method.h"
#include "../include/virtual_void/utillities/unnamed__.h"

namespace
{

struct MetaData {
  template <class P>
  constexpr explicit MetaData(std::in_place_type_t<P>)
      : type_info(typeid(P)) {}
  template <class P>
  constexpr explicit MetaData(std::in_place_type_t<std::shared_ptr<P>>)
      : type_info(typeid(P)) {}

  const std::type_info& type_info;
};

struct Shareable;
struct Writeable;
PRO_DEF_FREE_DISPATCH(FreeGetData, GetData_, GetData);
PRO_DEF_FREE_DISPATCH(FreeToString, ToString_, ToString);
PRO_DEF_FREE_DISPATCH(FreeToWriteable, ToWriteable_, ToWriteable);
PRO_DEF_FREE_DISPATCH(FreeToShareable, ToShareable_, ToShareable);

struct TypeData : pro::facade_builder
    ::support_copy<pro::constraint_level::nontrivial>
    ::add_convention<FreeGetData, void*() const>
    ::add_reflection<MetaData>
    ::build {};

struct BasicStringable : pro::facade_builder
    ::add_facade<TypeData, true>
    ::support_copy<pro::constraint_level::nontrivial>
    ::add_convention<FreeToString, std::string() const>
    ::build {};

template< typename C >
    pro::proxy<Writeable> ToWriteable_( const C& c );
struct Shareable : pro::facade_builder
    ::add_facade<BasicStringable, true>
    ::add_convention<FreeToWriteable, pro::proxy<Writeable>() const>
    ::support_copy<pro::constraint_level::nontrivial>
    ::build {};

template< typename C >
    pro::proxy<Shareable> ToShareable_( const C& c );
struct Writeable : pro::facade_builder
    ::add_facade<BasicStringable, true>
    ::add_convention<FreeToShareable, pro::proxy<Shareable>() const>
    ::support_copy<pro::constraint_level::nontrivial>
    ::build {};

template< typename C >
    pro::proxy<Writeable> ToWriteable_( const C& c )
    {
        auto cc= std::make_shared< C >( c );
        pro::proxy<Writeable> ccc( cc );
        return ccc;
    }

template< typename C >
    pro::proxy<Shareable> ToShareable_( const C& c )
    {
        auto cc= std::make_shared< C >( c );
        pro::proxy<Shareable> ccc( cc );
        return ccc;
    }

template< typename C >
    void* GetData_( C& c )
    {
        return &c;
    }
template< typename C >
    void* GetData_( std::shared_ptr< C >& c )
    {
        return c.get();
    }

struct X
{
    std::string s;
};
std::string ToString_( const X& x )
{
    return x.s;
}

TEST_CASE( "proxy prototype" ) 
{
    using namespace virtual_void;
    typeid_::domain testDomain;
    auto update = typeid_::open_method< void (void*, const std::string& ) >{ testDomain };
    auto __ = update.define< X >( +[]( X* x, const std::string& u ){ x->s += u; } );
    update.seal_for_runtime();

    auto o = X{ "hallo" };
    pro::proxy<Shareable> op{ &o };
    auto cp = ToWriteable( *op );
    REQUIRE( ToString( *op ) == "hallo" );
    REQUIRE( ToString( *cp ) == "hallo" );
    o.s = "world";
    REQUIRE( ToString( *op ) == "world" );
    REQUIRE( ToString( *cp ) == "hallo" );
    
    REQUIRE( pro::proxy_reflect<MetaData>(cp).type_info.name() == typeid( X ).name() );

    update( &o, "->updated" );
    REQUIRE( ToString_( o ) == "world->updated" );

    auto dataOriginal = GetData( *op );
    auto dataWriteable = GetData( *cp );
    REQUIRE( dataOriginal != dataWriteable ); 

    update( std::pair< const std::type_info&, void* >{ pro::proxy_reflect<MetaData>(cp).type_info, dataWriteable }, "->updated!!!" );
    REQUIRE( ToString( *cp ) == "hallo->updated!!!" );
}

}