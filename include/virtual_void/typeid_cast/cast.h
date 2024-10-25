#pragma once

#include "../forward.h"
#include "../erased/open_method/algorithm.h"

namespace virtual_void::typeid_cast {

template <template <typename> typename CONST, typename FOUND, typename FROM>
auto cast_implementation_(auto* from, const std::type_info& to) {
  typename CONST<void>::type found = nullptr;
  class_hierarchy::visit_class<FROM>(
      overload{[&]<typename C> {
                 if (!found && typeid(FROM) == to)
                   found = static_cast<CONST<void>::type>(from);
               },
               [&]<typename, typename B> {
                 if (!found)
                   found = cast_implementation_<CONST, FOUND, B>(
                       static_cast<CONST<B>::type>(from), to);
               }});
  return found;
}
struct const_cast_implementation {
  template <typename T>
  struct const_ {
    using type = const T*;
  };
  template <typename FROM>
  auto operator()(const FROM* from, const std::type_info& to) {
    return cast_implementation_<const_, typeid_const_void, FROM>(from, to);
  }
};
struct cast_implementation {
  template <typename T>
  struct non_const_ {
    using type = T;
  };
  template <typename FROM>
  auto operator()(FROM* from, const std::type_info& to) {
    return cast_implementation_<non_const_, typeid_void, FROM>(from, to);
  }
};
template <template <typename SIG> typename OPEN_METHOD>
using const_cast_method =
    OPEN_METHOD<const void*(const void*, const std::type_info& to)>;
template <template <typename SIG> typename OPEN_METHOD>
using cast_method = OPEN_METHOD<void*(const void*, const std::type_info& to)>;
void fill_const_cast_for(auto classes, auto& method) {
  virtual_void::open_method::fill_with_overloads(classes, method,
                                                 const_cast_implementation{});
}
template <typename... CLASSES>
void fill_const_cast_for(auto& method) {
  fill_const_cast_for(type_list<CLASSES...>{}, method);
}
void fill_cast_for(auto classes, auto& method) {
  fill_with_overloads(classes, method, cast_implementation{});
}
template <typename... CLASSES>
void fill_cast_for(auto& method) {
  fill_cast_for(type_list<CLASSES...>{}, method);
}
template <typename TO>
auto cast_to(const auto& cast, const auto& from) {
  if (auto void_ = cast(from, typeid(std::remove_const_t<TO>)))
    return static_cast<TO*>(void_);
  return static_cast<TO*>(nullptr);
}

}  // namespace virtual_void::typeid_cast