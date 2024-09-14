#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "proxy.h"

#include "SOM.h"

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
PRO_DEF_FREE_DISPATCH(FreeToString, ToString_, ToString);
PRO_DEF_FREE_DISPATCH(FreeToWriteable, ToWriteable_, ToWriteable);
PRO_DEF_FREE_DISPATCH(FreeToShareable, ToShareable_, ToShareable);

struct BasicStringable : pro::facade_builder
    ::support_copy<pro::constraint_level::nontrivial>
    ::add_convention<FreeToString, std::string() const>
    ::add_reflection<MetaData>
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

struct X
{
    std::string s;
};
std::string ToString_( const X& x )
{
    return x.s;
}

auto update = BitFactory::simple_open_method::declare< void >{};

namespace
{
    auto updateX = update.define< X >( []( X* x ){ x->s += "updated"; } );
}


int main()
{
    auto o = X{ "hallo" };
    pro::proxy<Shareable> op{ &o };
    auto cp = ToWriteable( *op );
    std::cout << ToString( *op ) << "\n";
    std::cout << ToString( *cp ) << "\n";
    o.s = "world";
    std::cout << ToString( *op ) << "\n";
    std::cout << ToString( *cp ) << "\n";
    
    std::cout << pro::proxy_reflect<MetaData>(cp).type_info.name() << "\n"; 

    update( &o );
    std::cout << ToString_( o ) << "\n";
}

