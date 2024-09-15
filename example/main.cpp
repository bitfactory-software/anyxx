#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "3rd_party/proxy.h"

#include "simple_open_method/define.h"
#include "utilities/type_list.h"
#include "class_hierarchy/class_hierarchy.h"

#include "class_hierarchy_test.h"
#include "simple_open_method_test.h"

constexpr std::uint64_t gms_hash_sdbm_64(const void *sP, size_t n, uint64_t param)
{
    uint64_t hash = 0;
    const unsigned char *s   = (const unsigned char*) sP;
    const unsigned char *end = s + n;

    // NB: 65599 is a prime that works well in practice
    // the parametrization isn't part of the original design,
    // i.e. this hash function wasn't designed as a parametized/seedable
    // hash function
    // however, this works good-enough to resolve collisions
    uint64_t k = 65599 + param;

    // NB: other versions of this function use some shifts
    //     however, modern optimizing compilers generate the same code,
    //     i.e. they replace the shifts with a multiplcation
    //     cf. https://godbolt.org/z/3Txh1n
    for (; s != end; ++s)
        hash = hash * k + *s;

    return hash;
}


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

    
    BitFactory::class_hierarchy::test_class_hierarchy();
    BitFactory::simple_open_method::test_simple_open_method();
}

