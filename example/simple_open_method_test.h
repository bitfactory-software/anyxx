#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

#include "../include/virtual_void/virtual_void.h"
#include "../include/virtual_void/virtual_void_ptr.h"

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

		void test_simple_open_method()
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
				build_runtime( testDomain );
				std::cout << "toSring.is_defined< D >() == " << std::boolalpha << (bool)toString.is_defined< D >() << "\n";
				call< D >( toString );
			}

			{
				domain testDomain;
				to_string_method toString( testDomain );
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( toString, ToString );
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
				fill_with_overloads< classes >( toString, ToString );
				build_runtime( testDomain );
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
				fill_with_overloads< classes >( any_factory, []< typename T >()->std::any
				{ 
					//std::cout << "construct any for " << typeid( T ).name() << std::endl; 
					return std::any( T() ); 
				});
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
				auto x = make_shared_const< D >( "shared hallo" );
				auto d = as< D >( x );
				std::cout << d->data << ", " << x.type().name() << std::endl; 
			}
			{
				auto const_void_factory = factory< shared_const() >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( const_void_factory, []< typename T >()->shared_const
				{ 
					return make_shared_const< T >( typeid( T ).name() ); 
				});
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
				auto d = as< D >( make_unique< D >( "unique hallo" ) );
				std::cout << d->data << ", " << d.type().name() << std::endl; 
			}
			{
				auto const_void_factory = factory< unique() >{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( const_void_factory, []< typename T >()->unique
				{ 
					return make_unique< T >( typeid( T ).name() ); 
				});
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