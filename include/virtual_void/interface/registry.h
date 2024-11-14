#pragma once

#include "../data/observer.h"
#include "../data/shared_const.h"
#include "../data/unique.h"
#include "../data/value.h"
#include "../open_method/via_type_info/factory.h"
#include "base.h"

namespace virtual_void::interface {

inline virtual_void::open_method::via_type_info::domain conversion_domain;

template <is_virtual_void TO, is_virtual_void FROM = TO>
using copy =
    virtual_void::open_method::via_type_info::factory<base<TO>(const FROM&)>;

template <is_virtual_void TO, is_virtual_void FROM = TO>
using move =
    virtual_void::open_method::via_type_info::factory<base<TO>(FROM&&)>;

template <typename META>
struct factories {

  copy<virtual_void::data::const_observer<META>> const_observer_copy;
  copy<virtual_void::data::mutable_observer<META>> mutable_observer_copy;
  copy<virtual_void::data::shared_const<META>> shared_const_copy;
  // copy<virtual_void::data::unique<META>> unique_copy;
  // copy<virtual_void::data::unique<META>,
  //      virtual_void::data::shared_const<META>>
  //     unique_copy_from_shared_const;

  // move<virtual_void::data::shared_const<META>,
  //      virtual_void::data::unique<META>>
  //     move_to_shared_const;
  // move<virtual_void::data::unique<META>,
  //      virtual_void::data::unique<META>>
  //     move_to_unique;
};

template <typename CLASS, typename META>
copy<virtual_void::data::const_observer<META>> const_observer_copy;

template <template <is_virtual_void> typename INTERFACE, typename CLASS,
          typename META>
void enable_const_observer_copy(copy<virtual_void::data::const_observer<META>>& const_observer_copy) {
  const_observer_copy
      .define<INTERFACE<virtual_void::data::const_observer<META>>>(
          +[](virtual_void::data::const_observer<META> const& from)
              -> base<virtual_void::data::const_observer<META>> {
            return INTERFACE<virtual_void::data::const_observer<META>>{
                *unchecked_unerase_cast<CLASS>(from)};
          });
};

// template <template< is_virtual_void > class INTERFACE, is_virtual_void VV>
// struct query_interface_;
// template<template< is_virtual_void > class INTERFACE, is_virtual_void VV>
// struct query_interface_<INTERFACE<VV>> {
//
// };

};  // namespace virtual_void::interface