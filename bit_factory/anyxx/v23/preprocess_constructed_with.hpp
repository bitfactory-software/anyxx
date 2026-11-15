#pragma once

#include <type_traits>

namespace anyxx {

template <template <typename...> typename Trait, typename... Args>
constexpr decltype(auto) preprocess_constructed_with(auto&& constructed_with) {
  using ConstructedWith = decltype(constructed_with);
  using trait_declaration_t = Trait<Args...>;
  if constexpr (requires(ConstructedWith constructed_with) {
                  {
                    trait_declaration_t::preprocess_constructed_with(
                        constructed_with)
                  };
                }) {
    return trait_declaration_t::preprocess_constructed_with(
        std::forward<ConstructedWith>(constructed_with));
  } else {
    return std::forward<ConstructedWith>(constructed_with);
  }
}

template <typename ConstructedWith, template <typename...> typename Trait,
          typename... Args>
using preprocess_constructed_with_t =
    std::decay_t<decltype(preprocess_constructed_with<Trait, Args...>(
        std::declval<ConstructedWith>()))>;

}  // namespace anyxx