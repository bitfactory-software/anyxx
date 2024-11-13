//#pragma once
//
//#include "../data/observer.h"
//#include "../data/unique.h"
//#include "../data/value.h"
//#include "../open_method/via_type_info/declare.h"
//#include "base.h"
//
//namespace virtual_void::interface {
//
//template <typename META>
//class i_table {
//  using namespace open_method::via_type_info;
//  using namespace data;
//  template <is_virtual_void TO, is_virtual_void FROM = TO>
//  using copy_method = declare<base<TO>(const FROM&, const std::type_info*)>;
//  template <is_virtual_void TO, is_virtual_void FROM = TO>
//  using move_method = declare<base<TO>(const FROM&, const std::type_info*)>;
//
//  copy_method<const_observer<META>> const_observer_copy;
//  copy_method<mutable_observer<META>> mutable_observer_copy;
//  copy_method<shared<META>> shared_copy;
//  copy_method<erased_unique<META>> unique_copy;
//  copy_method<erased_unique<META>, shared<META>> unique_copy;
//
//  move_method<shared<META>, erased_unique<META>> move_to_shared;
//  move_method<erased_unique<META>, unique<META>> move_to_unique;
//};
//
//
//template <typename META>
//class cast {
//  using namespace open_method::via_type_info;
//  using namespace data;
//  template <is_virtual_void TO, is_virtual_void FROM = TO>
//  using copy_method = declare<base<TO>(const FROM&, const std::type_info*)>;
//  template <is_virtual_void TO, is_virtual_void FROM = TO>
//  using move_method = declare<base<TO>(const FROM&, const std::type_info*)>;
//
//  copy_method<const_observer<META>> const_observer_copy;
//  copy_method<mutable_observer<META>> mutable_observer_copy;
//  copy_method<shared<META>> shared_copy;
////  copy_method<unique<META>, shared<META>> unique_copy;
//
//  //move_method<shared<META>, unique<META>> move_to_shared;
//  //move_method<unique<META>, unique<META>> move_to_unique;
//
//  template <typename CLASS, template <is_virtual_void> typename I>
//  friend void enable(cast<META>& this_) {
//	const_observer_copy.
//  }
//};
//};  // namespace virtual_void::interface