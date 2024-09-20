#pragma once

#include "../include/virtual_void/class_hierarchy/class_hierarchy.h"

namespace TestDomain
{
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
}

namespace class_hierarchy
{
	template<> struct describe< TestDomain::A1 >{ using bases = virtual_void::type_list<>; };
	template<> struct describe< TestDomain::A2 >{ using bases = virtual_void::type_list<>; };
	template<> struct describe< TestDomain::B1 >{ using bases = virtual_void::type_list< TestDomain::A1 >; };
	template<> struct describe< TestDomain::B2 >{ using bases = virtual_void::type_list< TestDomain::A2 >; };
	template<> struct describe< TestDomain::B3 >{ using bases = virtual_void::type_list< TestDomain::A1, TestDomain::A2 >; };
	template<> struct describe< TestDomain::C1 >{ using bases = virtual_void::type_list< TestDomain::B1 >; };
	template<> struct describe< TestDomain::C2 >{ using bases = virtual_void::type_list< TestDomain::B2 >; };
	template<> struct describe< TestDomain::C3 >{ using bases = virtual_void::type_list< TestDomain::B3 >; };
	template<> struct describe< TestDomain::C >{ using bases = virtual_void::type_list<>; };
	template<> struct describe< TestDomain::D >{ using bases = virtual_void::type_list< TestDomain::C, TestDomain::C3 >; };
};

