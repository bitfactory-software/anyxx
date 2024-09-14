#pragma once

#include <initializer_list>
#include <typeinfo>
#include <typeindex>
#include <map>

#include "type_list.h"

namespace class_hierarchy
{
	template< typename CLASS > struct describe;
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

	template< typename CLASS, bool deep >
	void declare( classes_with_bases& registry )
	{
		class_with_bases self_with_bases;
		self_with_bases.self = &typeid( CLASS );
		using bases = class_hierarchy::describe< CLASS >::bases;
		bases::for_each( [ & ]< typename BASE >()
			{ 
				self_with_bases.bases.emplace_back( &typeid( BASE ) );
				if constexpr( deep )
				{
					declare< BASE, deep >( registry );
				}
			});
		registry[ *self_with_bases.self ] = self_with_bases;
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