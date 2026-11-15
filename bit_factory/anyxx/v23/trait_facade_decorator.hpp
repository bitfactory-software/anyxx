#pragma once

#include <type_traits>

namespace anyxx {

struct dummy_t {};

template <typename Trait>
concept has_trait_facade_decorator = requires(dummy_t) {
  typename Trait::template trait_facade_decorator<dummy_t>;
};

template <typename Trait>
struct trait_facade_decorator {};
template <has_trait_facade_decorator Trait>
struct trait_facade_decorator<Trait>
    : Trait::template trait_facade_decorator<Trait> {};

}  // namespace anyxx