#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "include/catch.hpp"

#include "../include/virtual_void/virtual_void.h"

#include "class_hierarchy_test_hierarchy.h"

namespace virtual_void
{
	namespace
	{
		auto ToString = []( const auto* t )->std::string{ return typeid( *t ).name(); };  

		using to_string_method = method< std::string( const void* ) >;

		template< typename T > void call( const to_string_method& method )
		{ 
			T t;
			std::cout << method( &t ) << "\n";
		}


		TEST_CASE( "test_simple_open_method" ) 
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";

			using namespace TestDomain;

			std::cout << "\n";
			std::cout << "dispatch via void" << "\n";
			{
				domain testDomain;
				to_string_method toString( testDomain );
				
				toString.override_< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
				toString.seal();

				call< A1 >( toString );

				auto tv = to_typed_void( static_cast< A1* >( nullptr ) );
				std::cout << toString( tv ) << "\n";
				try
				{
					call< D >( toString );
					std::cout << "error: should not work!" << "\n";
				}
				catch( error& error )
				{
					std::cout << error.what() << " as expected." << "\n";
				}
			}
			{
				domain testDomain;
				to_string_method toString( testDomain );
				
				toString.override_< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
				declare_deep< D >( testDomain.classes );
				build_m_tables( testDomain );
				std::cout << "toSring.is_defined< D >() == " << std::boolalpha << (bool)toString.is_defined< D >() << "\n";
				call< D >( toString );
			}

			{
				domain testDomain;
				to_string_method toString( testDomain );
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads( classes{}, toString, ToString );
				seal( testDomain );
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ call< C >( toString ); }
					, [&]< typename C, typename B >	{}
					});
			}

			{
				domain testDomain;
				to_string_method toString( testDomain );
				using classes = type_list< D, C1, C2 >;
				class_hierarchy::declare_deep< D >( testDomain.classes );
				class_hierarchy::declare_deep< C1 >( testDomain.classes );
				class_hierarchy::declare_deep< C2 >( testDomain.classes );
				fill_with_overloads( classes{}, toString, ToString );
				build_m_tables( testDomain );
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				
					{	
						C c;
						auto virtual_void = to_virtual_void( &c );
						auto u = make_unique< C >();
						auto expected = typeid( C ).name();
						std::cout << "virtual_void dispatch for " << expected << ": "; 
						auto r = toString( u );
						auto r1 = toString( virtual_void );
						if( r != expected || r1 != expected )
							std::cout << "fail: " << r << ", " << r1;
						else
							std::cout << "OK";
						std::cout << std::endl;
					}
					, [&]< typename C, typename B >	{}
					});
			}

			{
				auto any_factory = factory< std::any() >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads( classes{}, any_factory, []< typename T >()->std::any
				{ 
					//std::cout << "construct any for " << typeid( T ).name() << std::endl; 
					return std::any( T() ); 
				});
				any_factory.seal();
				auto test = [ & ]< typename T >()
				{ 
					std::cout << "any_factory for " << typeid( T ).name() << ": "; 
					auto a = any_factory( typeid( T ) );
					if( a.type() != typeid( T ) )
						std::cout << "fail: " << a.type().name();
					else
						std::cout << "OK";
					std::cout << std::endl;

				};
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ test.template operator()< C >(); }
					, [&]< typename C, typename B >	{}
					});
			}

			{
				auto d = make_shared_const< D >( "shared hallo" );
				shared_const x = as< D >( d );
				auto d1 = as< D >( x );
				std::cout << d->data << ", " << x.type().name() << std::endl;
				static_assert( std::derived_from< D, A1 > );
				typed_shared_const< A1 > a1 = d1;
				typed_shared_const< A1 > a2 = A1{ "a2->OK" };
				typed_shared_const< A1 > a3{ std::in_place, "a3 in_place->OK" };
				A1 a1_pur{ "a1_pur" };
				typed_shared_const< A1 > a4{ a1_pur };
				auto& a1r = *a2;
				auto s1 = a1r.data;
				auto s = a2->data;
				std::cout << a2->data << ", " << a2.type().name() << std::endl;				
				std::cout << a3->data << ", " << a3.type().name() << std::endl;
				std::cout << a4->data << ", " << a4.type().name() << std::endl;

				auto c1 = make_unique< C >( "unique c1"); 
				std::cout << c1->data << ", " << c1.type().name() << std::endl;				
				auto c2 = typed_unique< C >( std::in_place, "unique c2" ); 
				std::cout << c2->data << ", " << c2.type().name() << std::endl;				
				auto c3 = typed_unique< C >( C{ "unique c3" } ); 
				std::cout << c3->data << ", " << c3.type().name() << std::endl;				
				auto c4 = std::move( c3 ); 
				std::cout << c4->data << ", " << c4.type().name() << std::endl;				
			}
			{
				auto const_void_factory = factory< shared_const() >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads( classes{}, const_void_factory, []< typename T >()->shared_const
				{ 
					return make_shared_const< T >( typeid( T ).name() ); 
				});
				const_void_factory.seal();
				auto test = [ & ]< typename T >()
				{ 
					std::cout << "shared_const_void_factory for " << typeid( T ).name() << ": "; 
					auto cv = const_void_factory( typeid( T ) );
					if( cv.type() != typeid( T ) )
						std::cout << "fail: " << cv.type().name();
					else
						std::cout << "OK";
					std::cout << ", ";
					auto tp = static_cast< const T* >( cv.data() );
					if( tp->data != typeid( T ).name() )
						std::cout << "fail: " << cv.type().name();
					else
						std::cout << "OK";
					std::cout << std::endl;
				};
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ test.template operator()< C >(); }
					, [&]< typename C, typename B >	{}
					});
			}
			{
				auto d1 = make_unique< D >( "unique hallo" );
				unique x{ std::move( d1 ) };
				auto d = as< D >( std::move( x ) );
				std::cout << d->data << ", " << d.type().name() << std::endl; 
			}
			{
				auto const_void_factory = factory< unique() >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads( classes{}, const_void_factory, []< typename T >()->unique
				{ 
					return make_unique< T >( typeid( T ).name() ); 
				});
				const_void_factory.seal();
				auto test = [ & ]< typename T >()
				{ 
					std::cout << "unique_void_factory for " << typeid( T ).name() << ": "; 
					auto cv = const_void_factory( typeid( T ) );
					if( cv.type() != typeid( T ) )
						std::cout << "fail: " << cv.type().name();
					else
						std::cout << "OK";
					std::cout << ", ";
					auto tp = static_cast< const T* >( cv.data() );
					if( tp->data != typeid( T ).name() )
						std::cout << "fail: " << cv.type().name();
					else
						std::cout << "OK";
					std::cout << std::endl;
				};
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ test.template operator()< C >(); }
					, [&]< typename C, typename B >	{}
					});
			}
		}
	}
}