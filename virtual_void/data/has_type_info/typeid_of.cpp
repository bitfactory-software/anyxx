#pragma once

#include <virtual_void/data/has_type_info/typeid_of.hpp>

namespace virtual_void {
using namespace data::has_type_info;
template <>
const std::type_info& typeid_of<const_observer>() {
  return typeid(const_observer);
}
template <>
const std::type_info& typeid_of<mutable_observer>() {
  return typeid(mutable_observer);
}
template <>
const std::type_info& typeid_of<shared_const>() {
  return typeid(shared_const);
}
template <>
const std::type_info& typeid_of<unique>() {
  return typeid(unique);
}
}  // namespace virtual_void
