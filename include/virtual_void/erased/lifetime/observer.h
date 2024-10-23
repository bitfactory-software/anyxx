#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/data/has_no_meta/has_no_meta.h"
#include "../../erased/data/observer_ptr.h"
#include "../../erased/virtual_void.h"

namespace virtual_void::erased {

template <typename VOID>
using observer_ptr =
    erased::data::observer_ptr<VOID, erased::data::has_no_meta>;
}

namespace virtual_void::erased {

template <typename DATA_PTR>
struct observer_data_trait
    : data_trait_base<DATA_PTR> {
  using void_t = DATA_PTR::void_t;
  static void_t value(const auto& ptr) { return ptr.value(); }
  static auto meta(const auto& ptr) { return ptr.meta(); }
  static bool has_value(const auto& ptr) {
    return static_cast<bool>(ptr.value());
  }
  template <typename V>
  static auto construct_from(V& v) {
    return observer_ptr<void_t>(v);
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires observer_ptr<void_t>::is_const
  {
    return observer_ptr<void_t>(v);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, ARG& arg) {
    return observer_ptr<void_t>(arg);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, const ARG& arg)
    requires observer_ptr<void_t>::is_const
  {
    return observer_ptr<void_t>(arg);
  }
  template <typename FROM>
  auto operator()(FROM& from) {
    return virtual_void<observer_ptr<void_t>>(from);
  }  // for migration to lifteime_handle! delete after migration!
  template <typename FROM>
  auto operator()(const FROM& from)
    requires observer_ptr<void_t>::is_const
  {
    return virtual_void<observer_ptr<void_t>>(from);
  }  // for migration to lifteime_handle! delete after migration!
};

template <typename VOID>
struct data_trait<observer_ptr<VOID>>
    : observer_data_trait<observer_ptr<VOID>> {};

}  // namespace virtual_void::erased

namespace virtual_void::erased {

template <typename VOID>
using observer = virtual_void<observer_ptr<VOID>>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

template <typename V, typename VOID>
using typed_observer = virtual_typed<V, observer_ptr<VOID>>;
template <typename V>
using typed_const_observer = typed_observer<V, void const*>;
template <typename V>
using typed_mutable_observer = typed_observer<V, void*>;

static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

static_assert(is_virtual_void<const_observer>);
static_assert(is_virtual_void<mutable_observer>);
static_assert(is_virtual_void<mutable_observer>);
static_assert(is_virtual_void<const_observer>);
static_assert(is_virtual_void<typed_const_observer<int>>);
static_assert(is_virtual_void<typed_mutable_observer<int>>);

}  // namespace virtual_void::erased