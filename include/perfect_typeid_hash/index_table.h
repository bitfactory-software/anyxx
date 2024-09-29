#pragma once

// original algorithm from: https://github.com/jll63/yomm2 THANKS!
// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <random>
#include <typeinfo>
#include <typeindex>
#include <concepts>
#include <optional>

namespace perfect_typeid_hash 
{

using type_id  = const std::type_info*;
using index_t = std::size_t;

const type_id unused_element = (type_id)-1;

template< typename TARGET >
    requires ( std::convertible_to< TARGET, bool > )
struct index_table
{
    using element_t = std::pair< type_id, TARGET >;
    using tabel_t = std::vector<element_t>;
 
    struct hash_index
    {
        std::size_t hash_mult = 0;
        std::size_t hash_shift = 0;
        std::size_t hash_length = 0;
        tabel_t table;
        index_t apply_formula(type_id type) const
        {
            return ( reinterpret_cast< std::size_t >( type ) * hash_mult) >> hash_shift;
        }
    };
    hash_index hash_;
    TARGET default_;

    TARGET at(type_id type) const
    {
        auto index = hash_.apply_formula(type);

        if (index >= hash_.hash_length || hash_.table[index].first != type) 
            return default_;

        return hash_.table[ index ].second;
    }
    TARGET operator[](type_id type) const
    {
        return at(type);
    }

    index_table( const index_table& ) = delete;
    index_table( const index_table&& ) = delete;

    explicit index_table( const std::vector< element_t >& elements,  TARGET default__ )
        : default_( default__ )
    {
        if( auto found = find_hash( elements, inital_sparse_factor( elements.size() ) ) )
            hash_ = *found;
        else
            throw "abort!";
    }

    auto static inital_sparse_factor( std::size_t element_count )
    {
       std::size_t sparse_factor = 1;
       for (auto size = element_count * 5 / 4; size; size /= 2)
           ++sparse_factor;
       return sparse_factor;
    }

    static std::optional<hash_index> find_hash( const std::vector< element_t >& elements, std::size_t sparse_factor )
    {
        for (std::size_t pass = 0; pass < 4; ++pass, ++sparse_factor) 
            if( auto found = find_hash_for_sparse_factor( elements, sparse_factor ) )
                return found;
        return{};
    }

    static std::optional<hash_index> find_hash_for_sparse_factor( const std::vector< element_t >& elements, std::size_t sparse_factor )
    {
        hash_index hash_index;
        hash_index.hash_shift = 8 * sizeof(type_id) - sparse_factor;
        hash_index.table.resize( std::size_t(1) << sparse_factor );
        hash_index.hash_length = 0;

        std::default_random_engine random_engine(13081963);
        std::uniform_int_distribution<std::uintptr_t> uniform_dist;

        for(std::size_t attempts = 0; attempts < 100000; ++attempts) 
            if( can_hash_input_values( elements, hash_index, ( uniform_dist(random_engine) | 1 ) ) )
                return hash_index;

        return {};
    }

    static std::optional<hash_index> can_hash_input_values( const std::vector< element_t >& elements, hash_index& hash_index, std::size_t hash_mult )
    {
        hash_index.hash_mult = hash_mult;
        std::fill(hash_index.table.begin(), hash_index.table.end(), std::pair{ unused_element, TARGET{} } );                
        for (const auto& element : elements ) 
            if( !can_hash_input_value( element, hash_index ) )
                return {};

        return hash_index;
    }
    static std::optional<hash_index> can_hash_input_value( const element_t& element, hash_index& hash_index )
    {
        auto index = hash_index.apply_formula(element.first);
        hash_index.hash_length = (std::max)(hash_index.hash_length, index+1);
        if( hash_index.table[index].first != unused_element )
            return {};
            
        hash_index.table[index] = element;
        return hash_index;
    }
};

}

