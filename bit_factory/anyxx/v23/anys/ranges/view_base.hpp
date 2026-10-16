#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/iterators.hpp>

namespace anyxx {

TRAIT_TEMPLATE(((AnyIterator)), view_base, dynamic_copyable, (),
                (ANY_FN_DEF(public, AnyIterator, begin, (), const,
                            [&x]() { return std::ranges::begin(x); })))


}  // namespace anyxx