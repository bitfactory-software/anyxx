#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "proxy.h"

#include "simple_open_method.h"
#include "type_list.h"
#include "class_hierarchy.h"

#include "class_hierarchy_test.h"
#include "simple_open_method_test.h"

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

struct A1 {};
struct A2 {};
struct B1 : A1 {};
struct B2 : A2 {};
struct B3 : A1, A2 {};
struct C1 : B1 {};
struct C2 : B2 {};
struct C3 : B3 {};
struct C {};
struct D : C, C3 {};

void test_type_hiearchy()
{
    BitFactory::classes_with_bases registry;
    DeclareBases< A1 >( registry );
    DeclareBases< A2 >( registry );
    DeclareBases< B1, A1 >( registry );
    DeclareBases< B2, A2 >( registry );
    DeclareBases< B3, A1, A2 >( registry );
    DeclareBases< C1, B1 >( registry );
    DeclareBases< C2, B2 >( registry );
    DeclareBases< C3, B3 >( registry );
    DeclareBases< C >( registry );
    DeclareBases< D, C, C3 >( registry );

    for( const auto& [ _, class_with_bases ] : registry )
    {
        std::cout << class_with_bases.self->name() << " : ";
        for( auto base : class_with_bases.bases )
            std::cout << base->name() << " ";
        std::cout << "\n";
    }
}

struct X
{
    std::string s;
};
std::string ToString_( const X& x )
{
    return x.s;
}

auto update = BitFactory::simple_open_method::declare< void, const std::string& >{};

namespace
{
    auto updateX = update.define< X >( []( X* x, const std::string& u ){ x->s += u; } );
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

    using types = BitFactory::type_list< int, double, X >;
    types::for_each( []< typename X >(){ std::cout << typeid( X ).name() << "\n"; } );

    update( &o, "->updated" );
    std::cout << ToString_( o ) << "\n";

    auto dataOriginal = GetData( *op );
    auto dataWriteable = GetData( *cp );
    std::cout << dataOriginal << ", " << dataWriteable << "\n"; 

    update( pro::proxy_reflect<MetaData>(cp).type_info, dataWriteable, "->updated!!!" );
    std::cout << ToString( *cp ) << "\n";

    
    BitFactory::class_hierarchy::test();
    BitFactory::simple_open_method::test();
}

