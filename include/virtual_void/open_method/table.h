#pragma once

#include <typeindex>
#include <unordered_map>

#include "../../include/perfect_typeid_hash/index_table.h"

#include "forward.h"

#include "error.h"

namespace virtual_void::open_method
{

class table
{
public:
	using dispatch_target_t = void(*)();
	using dispatch_target_index_t  = perfect_typeid_hash::index_table< dispatch_target_t >;
private:
	using method_map_t = std::unordered_map< std::type_info const *, dispatch_target_t >;
	method_map_t map_;
	dispatch_target_t default_ = reinterpret_cast< dispatch_target_t >( &throw_not_found );
public:
	static void throw_not_found(){ throw error( "no target specified." ); }
	void define_default( auto f )
	{
		default_ = reinterpret_cast< dispatch_target_t >( f );
	}
	auto get_default() const
	{
		return default_;
	}
	auto define( const std::type_info& register_type_info, auto f )
	{
		auto t = reinterpret_cast< dispatch_target_t >( f );
		if( is_defined( register_type_info ) )
			throw error( "Method for type already registered." );
		map_[ &register_type_info ] =  t;
		return definition{};
	}
	template< typename TARGET = dispatch_target_t >
	TARGET is_defined( const std::type_info& type_info ) const
	{
		if( auto found = map_.find( &type_info ); found != map_.end() )
			return reinterpret_cast< TARGET >( found->second );
		return nullptr;
	}
	dispatch_target_index_t make_lookup_table() const
	{
		return dispatch_target_index_t{ map_, get_default() };
	}
	struct definition{};

};

}