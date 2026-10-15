#pragma once

#include <bit_factory/v23/anys/range/iterators.hpp>

namespace anyxx {

TRAIT_TEMPLATE_(((AnyIterator)), view_base, dynamic_copyable, (),
                (ANY_FN_DEF(public, AnyIterator, begin, (), const,
                            [&x]() { return std::ranges::begin(x); })))


}  // namespace anyxx