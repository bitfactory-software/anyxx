#pragma once

#include <iostream>
#include <typeinfo>
#include <any>

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

		void test_simple_open_method()
		{
			std::cout << "\n";
			std::cout << __func__ << "\n";

			using namespace TestDomain;

			std::cout << "\n";
			std::cout << "dispatch via void" << "\n";
			{
				to_string_method toString;
				
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
				auto toString = to_string_method{};
				
				toString.override_< A1 >( +[]( const A1* x )->std::string{ return ToString( x ); } );
				class_hierarchy::classes_with_bases registry;
				declare_deep< D >( registry );
				interpolate( toString, registry );
				std::cout << "toSring.is_defined< D >() == " << std::boolalpha << (bool)toString.is_defined< D >() << "\n";
				toString.seal();
				call< D >( toString );
			}

			{
				auto toString = to_string_method{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( toString, ToString );
				toString.seal();
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				{ call< C >( toString ); }
					, [&]< typename C, typename B >	{}
					});
			}

			{
				auto toString = to_string_method{};
				using classes = type_list< D, C1, C2 >;
				fill_with_overloads< classes >( toString, ToString );
				toString.seal( 0 );
				build_v_tables< classes >( toString );
				class_hierarchy::visit_classes< classes >( 
					overload
					{ [&]< typename C >				
					{	
						C c;
						auto virtual_void = to_virtual_void( &c );
						auto expected = typeid( C ).name();
						std::cout << "virtual_void dispatch for " << expected << ": "; 
						auto r = toString( virtual_void );
						if( r != expected )
							std::cout << "fail: " << r;
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

		}
	}
}