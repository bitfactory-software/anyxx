#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/sentinels.hpp>
#include <bit_factory/anyxx/v23/anys/ranges/view_base.hpp>

namespace anyxx {

TRAIT_TEMPLATE(((Category), (Value), (Ref)), sized_view, view_base,
               ((Category), (Value), (Ref)),
               (FN_IMPL(public, (any<sized_sentinel, Value, Ref>), end, (),
                        const, () { return sentinel_for(self); })))

}  // namespace anyxx

namespace std::ranges {
template <typename Category, typename Value, typename Ref>
constexpr bool enable_view<anyxx::any<anyxx::sized_view, Category, Value, Ref>> =
true;
}  // namespace std::ranges