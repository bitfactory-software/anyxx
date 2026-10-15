#pragma once

#include <bit_factory/v23/anys/range/sentinels.hpp>
#include <bit_factory/v23/anys/range/view_base.hpp>

namespace anyxx {

TRAIT_TEMPLATE_(((AnyIterator)), view, view_base, ((AnyIterator)),
                (ANY_FN_DEF(public,
                            (any_<sentinel, typename AnyIterator::value_type,
                                  typename AnyIterator::reference>),
                            end, (), const,
                            [&x]() { return sentinel_for(x); })))

}  // namespace anyxx