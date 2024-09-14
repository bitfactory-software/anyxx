#pragma once

#include <initializer_list>
#include <typeinfo>
#include <typeindex>
#include <map>

#include "type_list.h"

namespace class_hierarchy
{
	template< typename CLASS > struct describe;

	template< typename CLASS, bool deep >
	void visit( auto visit_class, auto visit_base )
	{
        visit_class.template operator()< CLASS >();
		using bases = class_hierarchy::describe< CLASS >::bases;
		bases::for_each( [ & ]< typename BASE >()
		{ 
	        visit_base.template operator()< CLASS, BASE >();
			if constexpr( deep )
			{
				visit< BASE, deep >( visit_class, visit_base );
			}
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

	template< typename CLASS, bool deep = true >
	void declare( classes_with_bases& registry )
	{
		class_hierarchy::visit< CLASS, deep >
			( [&]< typename C >				{ registry[ typeid( C ) ].self = &typeid( C ); }
			, [&]< typename C, typename B >	{ registry[ typeid( C ) ].bases.emplace_back( &typeid( B ) ); }
			);
	}
	template< typename CLASSES, bool deep = true >
	void declare_all( classes_with_bases& registry )
	{
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