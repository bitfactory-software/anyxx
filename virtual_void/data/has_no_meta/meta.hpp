#pragma once

#include <virtual_void/utillities/VIRTUAL_DESTRUCTOR_FOR_DEBUGGING.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data::has_no_meta {

struct meta {
  const auto* get_meta() const { return this; }
  template <typename T>
  meta(std::in_place_type_t<T>) {}
  template <is_meta META>
  meta(const META&) {}
  meta() = default;
  meta(meta const&) = default;
  meta(meta&&) = default;
  meta& operator=(meta const&) = default;
  meta& operator=(meta&&) = default;
  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING ~meta() = default;
};

inline auto get_std_type_info(meta const& m) { return nullptr; }

}  // namespace virtual_void::data::has_no_meta