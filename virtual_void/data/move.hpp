#pragma once

#include <virtual_void/data/observer.hpp>

namespace virtual_void::data {

template <is_erased_data TO, is_erased_data FROM>
struct move_converter;

template <typename TO, typename FROM>
concept move_convertable_from =
    is_erased_data<FROM> && is_erased_data<TO> && requires(FROM&& f) {
      { move_converter<TO, FROM>{}(std::move(f)) } -> std::same_as<TO>;
    };

template <is_erased_data TO, is_erased_data FROM>
struct move_converter {
  TO operator()(FROM&& from) { return std::move(from); }
};

template <voidness VOIDNESS, is_erased_data FROM>
requires (!voidness<FROM>)
struct move_converter<observer<VOIDNESS>, FROM> {
  auto operator()(auto&&) { static_assert(false, "no move to observer!"); }
};

template <voidness TO, voidness FROM>
requires const_correct_call<is_const_void<TO>, is_const_void<FROM>>
struct move_converter<observer<TO>, FROM> {
  TO operator()(auto&& from) { return from; }
};

template <typename TO, typename FROM>
  requires move_convertable_from<TO, std::decay_t<FROM>>
TO move_convert_to(FROM&& from) {
  return move_converter<TO, FROM>{}(std::move(from));
}

};  // namespace virtual_void::data