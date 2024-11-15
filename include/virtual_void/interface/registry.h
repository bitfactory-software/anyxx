#pragma once

#include "../data/observer.h"
#include "../data/shared_const.h"
#include "../data/unique.h"
#include "../data/value.h"
#include "../open_method/via_type_info/factory.h"
#include "base.h"

namespace virtual_void::interface {

inline virtual_void::open_method::via_type_info::domain query_interface_domain;

template <is_virtual_void TO, is_virtual_void FROM = TO>
using move =
    virtual_void::open_method::via_type_info::factory<base<TO>(FROM&&)>;

template <is_virtual_void TO, is_virtual_void FROM = TO>
using copy_factory_method =
    virtual_void::open_method::via_type_info::factory<base<TO>(const FROM&)>;
template <typename CLASS, is_virtual_void TO, is_virtual_void FROM = TO>
copy_factory_method<TO, FROM> copy{query_interface_domain};

template <is_virtual_void TO, is_virtual_void FROM = TO>
using find_copy_factory_method =
    virtual_void::open_method::via_type_info::factory<copy_factory_method<TO, FROM> const&()>;
template <is_virtual_void TO, is_virtual_void FROM = TO>
find_copy_factory_method<TO, FROM> find_copy{query_interface_domain};

//template <typename CLASS, typename META>
//copy<virtual_void::data::mutable_observer<META>> mutable_observer_copy;
//template <typename CLASS, typename META>
//copy<virtual_void::data::shared_const<META>> shared_const_copy;
// copy<virtual_void::data::unique<META>> unique_copy;
// copy<virtual_void::data::unique<META>,
//      virtual_void::data::shared_const<META>>
//      unique_copy_from_shared_const;

// move<virtual_void::data::shared_const<META>,
//      virtual_void::data::unique<META>>
//     move_to_shared_const;
// move<virtual_void::data::unique<META>,
//      virtual_void::data::unique<META>>
//     move_to_unique;

template <template <is_virtual_void> typename INTERFACE, typename CLASS,
          typename META>
auto default_const_observer_interface() {
  return +[](virtual_void::data::const_observer<META> const& from)
             -> base<virtual_void::data::const_observer<META>> {
    return INTERFACE<virtual_void::data::const_observer<META>>{
        *unchecked_unerase_cast<CLASS>(from)};
  };
}
template <template <is_virtual_void> typename TO_INTERFACE, typename CLASS,
          typename META, typename IMPLEMENATAION>
void enable_const_observer_copy(IMPLEMENATAION impl) {
  using target_virtual_void_t =
      virtual_void::data::const_observer<META>;
    using target_interface_t =
      TO_INTERFACE<target_virtual_void_t>;
  copy<CLASS, target_virtual_void_t>.define<target_interface_t>(impl);
  if (!find_copy<target_virtual_void_t>.is_defined<CLASS>())
    find_copy<target_virtual_void_t>.define<CLASS>(
        +[]() -> auto const& { return copy<CLASS, target_virtual_void_t>; });
};

template <template <is_virtual_void> typename TO_INTERFACE, typename CLASS,
          typename META>
void enable_const_observer_copy() {
  enable_const_observer_copy<TO_INTERFACE, CLASS, META>(
      default_const_observer_interface<TO_INTERFACE, CLASS, META>());
}

//template <is_virtual_void TO, is_virtual_void FROM>
//const auto& copy_factory_(const std::type_info& type_info) {
//  using meta_t = typename virtual_void_trait<TO>::meta_t;
//  using from_meta_t = virtual_void_trait<FROM>::meta_t;
//  static_assert(std::is_same_v<meta_t, from_meta_t>);
//  return copies < TO, FROM
//
//         if constexpr (std::same_as<TO, FROM>) {
//    if constexpr (std::same_as<virtual_void::data::const_observer<meta_t>,
//                               TO>) {
//      return const_observer_copies<meta_t>(type_info);
//    } else if constexpr (std::same_as<
//                             virtual_void::data::mutable_observer<meta_t>,
//                             TO>) {
//    } else if constexpr (std::same_as<virtual_void::data::shared_const<meta_t>,
//                                      TO>) {
//    }
//  }
//  else {
//    static_assert(false);
//    return {};
//  }
//}

// template <template< is_virtual_void > class INTERFACE,
// is_virtual_void VV> struct query_interface_; template<template<
// is_virtual_void > class INTERFACE, is_virtual_void VV> struct
// query_interface_<INTERFACE<VV>> {
//
// };

// template<typename

template <typename TO_INTERFACE, typename FROM_INTERFACE>
FROM_INTERFACE query_interface(const FROM_INTERFACE& from_interface) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  using vv_from_t = typename FROM_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  static_assert(is_virtual_void<vv_from_t>);
  const auto& vv_from = get_virtual_void(from_interface);
  auto const& type_info = *get_meta(vv_from)->type_info();
  auto const& copy = find_copy<vv_to_t, vv_from_t>(type_info);
  auto base = copy.construct<TO_INTERFACE>(vv_from);
  return static_v_table_cast<FROM_INTERFACE>(base);
}
};  // namespace virtual_void::interface