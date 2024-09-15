#pragma once

#include <initializer_list>
#include <typeinfo>
#include <typeindex>
#include <map>

#include "type_list.h"
#include "overload.h"

namespace class_hierarchy
{
	template< typename CLASS > struct describe;

};

namespace BitFactory::class_hierarchy
{
	template< typename CLASS, bool deep = true >
	void visit_class( auto visitor )
	{
        visitor.template operator()< CLASS >();
		using bases = ::class_hierarchy::describe< CLASS >::bases;
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

	using bases_t =	std::vector< const std::type_info* >;
	struct class_with_bases
	{
		const std::type_info* self;
		bases_t bases;
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

	void visit_hierarchy( const std::type_index& self, const classes_with_bases& classes_with_bases, auto visitor );
	void visit_bases( const bases_t& bases, const classes_with_bases& classes_with_bases, auto visitor )
	{
		for( auto base : bases )
			visit_hierarchy( *base, classes_with_bases, visitor );
	}
	void visit_hierarchy( const std::type_index& self, const classes_with_bases& classes_with_bases, auto visitor )
	{
		auto found = classes_with_bases.find( self );
		if( found == classes_with_bases.end() )
			throw std::runtime_error("class not registered.");
		visitor( *found->second.self );
		visit_bases( found->second.bases, classes_with_bases, visitor );
	}
}