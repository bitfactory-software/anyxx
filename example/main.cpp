#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../include/proxy/proxy.h"

#include "../include/virtual_void/method.h"
#include "../include/virtual_void/class_hierarchy/class_hierarchy.h"
#include "../include/virtual_void/utilities/naive_any_function.h"
#include "../include/virtual_void/utilities/naive_any_value.h"

#include "class_hierarchy_test.h"
#include "simple_open_method_test.h"
#include "typeid_cast_test.h"

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

auto update = virtual_void::method< void (void*, const std::string& ) >{};

namespace
{
    auto updateX = update.override_< X >( +[]( X* x, const std::string& u ){ x->s += u; } );
}

template< template< typename, typename... > class any_function, typename any_value > void test_any()
{
    std::cout << "any_function..." << std::endl << std::endl;

    any_function< std::string( int ) > toString1 { []( int i ){ return std::to_string( i ) + "1"; } };
    std::cout << "1: " << toString1( 4711 ) << std::endl;

    auto x = []( int i )->std::string{ return std::to_string( i ) + "x"; };
    std::cout << x( 4711 ) << std::endl;

    auto y = []( int i )->std::string{ return std::to_string( i ) + "y"; };
    std::cout << y( 4711 ) << std::endl;

    any_function< std::string( int ) > toString2 ( x );
    std::cout << "2: " << toString2( 4711 ) << std::endl;

    any_function< std::string( int ) > toString3 ( toString1 );
    std::cout << "3: " << toString3( 4711 ) << std::endl;

    toString2 = toString3;
    std::cout << "2: " << toString2( 4711 ) << std::endl;
    std::cout << "3: " << toString3( 4711 ) << std::endl;

    toString2 = y;
    std::cout << "2: " << toString2( 4711 ) << std::endl;

    std::cout << "any_value ..." << std::endl << std::endl;

    any_value a0{ 3.14 };
    std::cout << "a1 " << a0.type().name() << std::endl;
    try
    {
        any_value_cast< int >( a0 );
        std::cout << "error! must throw!" << std::endl;
    }
    catch( std::runtime_error& e)
    {
        std::cout << "cathed: " << e.what() << std::endl;
    }
    any_value a1 = 4711;
    std::cout << "a1 " << a1.type().name() << std::endl;
    //auto helloWorld = "hello world";
    //any_value a2 = helloWorld;
    //std::cout << "a2 " << a2.type().name() << std::endl;
    any_value a3 = std::string{ "hello any_value" };
    std::cout << "a3 " << a3.type().name() << std::endl;
    any_value a4 = a3;
    std::cout << "a4 " << a4.type().name() << std::endl;

    std::cout << "a1 " << any_value_cast< int >( a1 ) << std::endl;
    //std::cout << "a2 " << any_value_cast< decltype( helloWorld ) >( a2 ) << std::endl;
    //std::cout << "a2 " << any_value_cast< const char* >(a2) << std::endl;
    std::cout << "a3 " << any_value_cast< std::string >( a3 ) << std::endl;
    std::cout << "a4 " << any_value_cast< std::string >( a4 ) << std::endl;
}

template< typename T > void trace_alignof()
{
    std::cout << "alignof(" << typeid( T ).name() << "): " << alignof( T ) << std::endl;

}
int main()
{
    trace_alignof< const char* >();
    trace_alignof< std::string >();
    trace_alignof< int >();
    trace_alignof< double >();

    auto o = X{ "hallo" };
    pro::proxy<Shareable> op{ &o };
    auto cp = ToWriteable( *op );
    std::cout << ToString( *op ) << "\n";
    std::cout << ToString( *cp ) << "\n";
    o.s = "world";
    std::cout << ToString( *op ) << "\n";
    std::cout << ToString( *cp ) << "\n";
    
    std::cout << pro::proxy_reflect<MetaData>(cp).type_info.name() << "\n"; 

    using types = virtual_void::type_list< int, double, X >;
    types::for_each( []< typename X >(){ std::cout << typeid( X ).name() << "\n"; } );
    update.seal();
    update( &o, "->updated" );
    std::cout << ToString_( o ) << "\n";

    auto dataOriginal = GetData( *op );
    auto dataWriteable = GetData( *cp );
    std::cout << dataOriginal << ", " << dataWriteable << "\n"; 

    update( std::pair< const std::type_info&, void* >{ pro::proxy_reflect<MetaData>(cp).type_info, dataWriteable }, "->updated!!!" );
    std::cout << ToString( *cp ) << "\n";

    
    virtual_void::class_hierarchy::test_class_hierarchy();
    virtual_void::test_simple_open_method();
    typeid_cast_test();

    test_any< naive::any_function, naive::any_value >();

    return 0;
}

