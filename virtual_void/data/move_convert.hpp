#pragma once

#undef interface

#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/data/has_meta_runtime/value.hpp>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/meta/i_table.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <is_virtual_void TO, is_virtual_void FROM>
struct move_converter {
  TO operator()(FROM&& from) { return std::move(from); }
};

template <voidness VOIDNESS, typename META, is_virtual_void FROM>
struct move_converter<observer<VOIDNESS, META>, FROM> {
  auto operator()(auto&&) { static_assert(false, "no move to observer!"); }
};

template <typename TO, typename FROM>
TO move_convert_to(FROM&& from) {
  return move_converter<TO, FROM>{}(std::move(from));
}

};  // namespace virtual_void::data