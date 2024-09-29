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

    std::size_t hash_mult = 0;
    std::size_t hash_shift = 0;
    std::size_t hash_length = 0;

    std::vector<element_t> table;


    TARGET default_;

    index_t apply_formula(type_id type) const
    {
        return ( reinterpret_cast< std::size_t >( type ) * hash_mult) >> hash_shift;
    }
    TARGET at(type_id type) const
    {
        auto index = apply_formula(type);

        if (index >= hash_length || table[index].first != type) 
            return default_;

        return table[ index ].second;
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
        const auto element_count = elements.size();

        std::size_t magnitude = 1;

        for (auto size = element_count * 5 / 4; size; size /= 2)
            ++magnitude;

        for (std::size_t pass = 0; pass < 4; ++pass, ++magnitude) 
        {
            hash_shift = 8 * sizeof(type_id) - magnitude;
            table.resize( std::size_t(1) << magnitude );
            hash_length = 0;

            if( find_hash_mult_for_magnitude(elements) )
                return;
        }

        throw "abort!";
    }

    bool find_hash_mult_for_magnitude( const std::vector< element_t >& elements )
    {
        std::default_random_engine random_engine(13081963);
        std::uniform_int_distribution<std::uintptr_t> uniform_dist;
        for(std::size_t attempts = 0; attempts < 100000; ++attempts) 
        {
            std::fill(table.begin(), table.end(), std::pair{ unused_element, TARGET{} } );                
            hash_mult = uniform_dist(random_engine) | 1;
            if( hash_mult_for_input_values( elements ) )
                return true;
        }
        return false;
    }

    bool hash_mult_for_input_values( const std::vector< element_t >& elements )
    {
        for (const auto [ type, target ] : elements ) 
        {
            auto index = apply_formula(type);
            hash_length = (std::max)(hash_length, index+1);

            if( table[index].first == unused_element )
                table[index] = std::pair{ type, target };
            else
                return false;
        }
        return true;
    }
};

}

