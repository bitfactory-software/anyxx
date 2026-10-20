#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/iterators.hpp>
#include <bit_factory/anyxx/anys/ranges/iterator_categories.hpp>
#include <ranges>

namespace anyxx {

TRAIT_TEMPLATE(((Category), (Value), (Ref)), view_base, dynamic_copyable, (),
               (FN_IMPL(public, (any<Category::template iterator, Value, Ref>), begin, (), const,
                        [&self]() { return std::ranges::begin(self); })))

}  // namespace anyxx

