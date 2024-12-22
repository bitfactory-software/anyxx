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
struct copy_converter;

template <is_virtual_void FROM>
struct copy_converter<has_no_meta::const_observer, FROM> {
  auto operator()(const auto& from) {
    return has_no_meta::const_observer{virtual_void::get_data(from),
                                       has_no_meta::meta{}};
  }
};
template <is_virtual_void FROM>
  requires(!is_const_data<FROM>)
struct copy_converter<has_no_meta::mutable_observer, FROM> {
  auto operator()(const auto& from) {
    return has_no_meta::mutable_observer{virtual_void::get_data(from),
                                         has_no_meta::meta{}};
  }
};
template <is_virtual_void FROM>
  requires std::same_as<meta_t<FROM>, has_meta_runtime::meta>
struct copy_converter<has_meta_runtime::const_observer, FROM> {
  auto operator()(const auto& from) {
    return has_meta_runtime::const_observer{virtual_void::get_data(from),
                                       *virtual_void::get_meta(from)};
  }
};
template <is_virtual_void FROM>
  requires std::same_as<meta_t<FROM>, has_meta_runtime::meta>
struct copy_converter<has_meta_runtime::mutable_observer, FROM> {
  auto operator()(const auto& from) {
    return has_meta_runtime::mutable_observer{virtual_void::get_data(from),
                                         *virtual_void::get_meta(from)};
  }
};
template <>
struct copy_converter<has_meta_runtime::shared_const, has_meta_runtime::shared_const> {
  auto operator()(const auto& from) {
    return from;
  }
};
template <>
struct copy_converter<has_meta_runtime::value, has_meta_runtime::value> {
  auto operator()(const auto& from) {
    return from;
  }
};
template <is_virtual_void FROM>
  requires std::same_as<meta_t<FROM>, has_meta_runtime::meta>
struct copy_converter<has_meta_runtime::unique, FROM> {
  auto operator()(const auto& from) {
    return get_meta(from)->type_info()->copy_construct(get_data(from));
  }
};

template <typename TO, typename FROM>
TO copy_convert_to(FROM const& from) {
  return copy_converter<TO, FROM>{}(from);
}

};  // namespace virtual_void::data