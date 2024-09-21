#pragma once

#include "../include/virtual_void/method.h"

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

namespace virtual_void::class_hierarchy
{
	using namespace TestDomain;
	template<> struct describe< A1 >{ using bases = none; };
	template<> struct describe< A2 >{ using bases = none; };
	template<> struct describe< B1 >{ using bases = are< A1 >; };
	template<> struct describe< B2 >{ using bases = are< A2 >; };
	template<> struct describe< B3 >{ using bases = are< A1, A2 >; };
	template<> struct describe< C1 >{ using bases = are< B1 >; };
	template<> struct describe< C2 >{ using bases = are< B2 >; };
	template<> struct describe< C3 >{ using bases = are< B3 >; };
	template<> struct describe< C >{ using bases = none; };
	template<> struct describe< D >{ using bases = are< C, C3 >; };
};

