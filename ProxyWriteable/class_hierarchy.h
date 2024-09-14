#pragma once

#include <initializer_list>
#include <typeinfo>
#include <typeindex>
#include <map>

#include "type_list.h"

namespace BitFactory
{
	using type_infos = std::initializer_list< const std::type_info& >;

	struct class_with_bases
	{
		std::type_info* self;
		std::vector< std::type_info* > bases;
	};

	using classes_with_bases = std::map< std::type_index, class_with_bases >;

	template< typename SELF, typename... BASES >
	nullptr_t DeclareBases( classes_with_bases& registry )
	{
		class_with_bases self_with_bases;
		self_with_bases.self = const_cast< std::type_info* >( &typeid( SELF ) );
		type_list< BASES... >::for_each( [ &self_with_bases ]< typename BASE >()
			{ 
				self_with_bases.bases.push_back( const_cast< std::type_info* >( &typeid( BASE ) ) ); 
			});
		registry[ *self_with_bases.self ] = self_with_bases;
		return {};
	}
}