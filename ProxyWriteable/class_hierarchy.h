#pragma once

#include <initializer_list>
#include <typeinfo>
#include <typeindex>
#include <map>

#include "type_list.h"

namespace BitFactory
{
	template<typename ... Ts> struct overload : Ts ... { using Ts::operator() ...; };
	template<class... Ts> overload(Ts...) -> overload<Ts...>;
}

namespace class_hierarchy
{
	template< typename CLASS > struct describe;

	template< typename CLASS, bool deep = true >
	void visit_class( auto visitor )
	{
        visitor.template operator()< CLASS >();
		using bases = class_hierarchy::describe< CLASS >::bases;
		bases::for_each( [ & ]< typename BASE >()
		{ 
	        visitor.template operator()< CLASS, BASE >();
			if constexpr( deep )
			{
				visit_class< BASE, deep >( visitor );
			}
		});
	}
	template< typename CLASSES, bool deep = true >
	void visit_classes( auto visitor )
	{
		CLASSES::for_each( [ & ]< typename CLASS >()
		{ 
	        visit_class< CLASS, deep >( visitor );
		});
	}
};

namespace BitFactory
{
	using type_infos = std::initializer_list< const std::type_info& >;

	struct class_with_bases
	{
		const std::type_info* self;
		std::vector< const std::type_info* > bases;
	};

	using classes_with_bases = std::map< std::type_index, class_with_bases >;

	auto declare_visitor( classes_with_bases& registry )
	{
		return overload
			{ [&]< typename C >				{ registry[ typeid( C ) ].self = &typeid( C ); }
			, [&]< typename C, typename B >	{ registry[ typeid( C ) ].bases.emplace_back( &typeid( B ) ); }
			};	
	}
	template< typename CLASS, bool deep = true >
	void declare( classes_with_bases& registry )
	{
		class_hierarchy::visit_class< CLASS, deep >( declare_visitor( registry ) );
	}
	template< typename CLASSES, bool deep = true >
	void declare_all( classes_with_bases& registry )
	{
		class_hierarchy::visit_classes< CLASSES, deep >( declare_visitor( registry ) );
	}
	template< typename CLASS >
	void declare_deep( classes_with_bases& registry )
	{
		declare< CLASS, true >( registry );
	}

	template< typename CLASS >
	void declare_shallow( classes_with_bases& registry )
	{
		declare< CLASS, false >( registry );
	}
}