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

using type_id   = const std::type_info*;
using index_t     = std::size_t;

template< typename TARGET >
    requires ( std::convertible_to< TARGET, bool > )
struct index_table
{
    using element_t = std::pair< type_id, TARGET >;

    std::size_t hash_mult = 0;
    std::size_t hash_shift = 0;
    std::size_t hash_length = 0;
    std::size_t hash_min = 0;
    std::size_t hash_max = 0;

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

        std::default_random_engine rnd(13081963);
        std::size_t total_attempts = 0;
        std::size_t M = 1;

        for (auto size = element_count * 5 / 4; size >>= 1;)
            ++M;

        std::uniform_int_distribution<std::uintptr_t> uniform_dist;

        for (std::size_t pass = 0; pass < 4; ++pass, ++M) 
        {
            hash_shift = 8 * sizeof(type_id) - M;
            auto hash_size = 1 << M;

            bool found = false;
            std::size_t attempts = 0;
            table.resize(hash_size);
            hash_length = 0;

            const type_id unused_element = (type_id)-1;

            while (!found && attempts < 100000) 
            {
                std::fill(table.begin(), table.end(), std::pair{ unused_element, TARGET{} } );
                ++attempts;
                ++total_attempts;
                found = true;
                hash_mult = uniform_dist(rnd) | 1;

                for (const auto [ type, target ] : elements ) 
                {
                    auto index = apply_formula(type);
                    hash_min = (std::min)(hash_min, index);
                    hash_max = (std::max)(hash_max, index);

                    if (found = ( table[index].first == unused_element))
                        table[index] = std::pair{ type, target };
                    else
                        break;
                }
            }

            if (!found)
                continue;

            hash_length = hash_max + 1;
            return;
        }

        throw "abort!";
    }
};

}

