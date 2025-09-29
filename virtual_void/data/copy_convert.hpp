#pragma once

#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <is_erased_data TO, is_erased_data FROM>
struct copy_converter;

template <is_erased_data FROM>
  requires(!is_const_data<FROM>)
struct copy_converter<mutable_observer, FROM> {
  auto operator()(const auto& from, auto const& get_meta_data) {
    return mutable_observer{virtual_void::get_data(from)};
  }
};
template <is_erased_data FROM>
struct copy_converter<const_observer, FROM> {
  auto operator()(const auto& from, auto const& get_meta_data) {
    return const_observer{virtual_void::get_data(from)};
  }
};
template <>
struct copy_converter<shared_const, shared_const> {
  auto operator()(const auto& from, auto const& get_meta_data) {
    return from;
  }
};
template <is_erased_data FROM>
struct copy_converter<unique, FROM> {
  auto operator()(const auto& from, auto const& get_meta_data) {
    return get_meta_data.copy_construct(get_data(from));
  }
};

template <typename TO, typename FROM>
TO copy_convert_to(FROM const& from, auto const& get_meta_data) {
  return copy_converter<TO, FROM>{}(from, get_meta_data);
}

};  // namespace virtual_void::data