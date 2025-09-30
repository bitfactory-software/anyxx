#pragma once

#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>

namespace virtual_void::data {

template <is_erased_data TO, is_erased_data FROM>
struct cast_converter;

template <typename TO, typename FROM>
concept cast_convertable_from =
    is_erased_data<FROM> && is_erased_data<TO> && requires(FROM f) {
      { cast_converter<TO, FROM>{}(f) } -> std::same_as<TO>;
    };

template <is_erased_data FROM>
  requires(!is_const_data<FROM>)
struct cast_converter<mutable_observer, FROM> {
  auto operator()(const auto& from) { return mutable_observer{get_data(from)}; }
};
template <is_erased_data FROM>
struct cast_converter<const_observer, FROM> {
  auto operator()(const auto& from) { return const_observer{get_data(from)}; }
};
template <>
struct cast_converter<shared_const, shared_const> {
  auto operator()(const auto& from) { return from; }
};

template <typename TO, typename FROM>
  requires cast_convertable_from<TO, FROM>
TO cast_to(FROM const& from) {
  return cast_converter<TO, FROM>{}(from);
}

};  // namespace virtual_void::data