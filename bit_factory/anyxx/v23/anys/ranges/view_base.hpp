#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/iterators.hpp>
#include <bit_factory/anyxx/anys/ranges/iterator_categories.hpp>
#include <ranges>

namespace anyxx {

TRAIT_TEMPLATE_EX(((Category), (Value), (Ref))
    , view_base
      , dynamic_copyable, (),
      (FN_IMPL(public, (any<Category::template iterator, Value, Ref>), begin, (), const,
        () { return std::ranges::begin(self); })),
      ,,,
      (template <typename Any>
        using trait_facade_decorator = std::ranges::view_interface<Any>;)
    )

}  // namespace anyxx

