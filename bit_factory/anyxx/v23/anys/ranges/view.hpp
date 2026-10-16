#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/sentinels.hpp>
#include <bit_factory/anyxx/v23/anys/ranges/view_base.hpp>

namespace anyxx {

TRAIT_TEMPLATE(((AnyIterator)), view, view_base, ((AnyIterator)),
                (ANY_FN_DEF(public,
                            (any_<sentinel, typename AnyIterator::value_type,
                                  typename AnyIterator::reference>),
                            end, (), const,
                            [&x]() { return sentinel_for(x); })))

}  // namespace anyxx