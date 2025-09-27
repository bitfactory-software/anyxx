#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <is_erased_data TO, is_erased_data FROM>
struct move_converter {
  TO operator()(FROM&& from) { return std::move(from); }
};

template <voidness VOIDNESS, typename META, is_erased_data FROM>
struct move_converter<observer<VOIDNESS, META>, FROM> {
  auto operator()(auto&&) { static_assert(false, "no move to observer!"); }
};

template <typename TO, typename FROM>
TO move_convert_to(FROM&& from) {
  return move_converter<TO, FROM>{}(std::move(from));
}

};  // namespace virtual_void::data