#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/data/has_no_meta/has_no_meta.h"
#include "../../erased/data/observer_ptr.h"
#include "../../erased/lifetime_handle.h"

namespace virtual_void::erased {

template <typename VOID>
using observer_ptr =
    erased::data::observer_ptr<VOID, erased::data::has_no_meta>;
}

namespace virtual_void::erased {

template <typename VOID>
struct data_trait<observer_ptr<VOID>> : data_trait_base<observer_ptr<VOID>> {
  using void_t = VOID;
  static void_t value(const auto& ptr) { return ptr.value(); }
  static auto meta(const auto& ptr) { return ptr.meta(); }
  static bool has_value(const auto& ptr) {
    return static_cast<bool>(ptr.value());
  }
  template <typename V>
  static auto construct_from(V& v) {
    return observer_ptr<VOID>(v);
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires observer_ptr<VOID>::is_const
  {
    return observer_ptr<VOID>(v);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, ARG& arg) {
    return observer_ptr<VOID>(arg);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, const ARG& arg)
    requires observer_ptr<VOID>::is_const
  {
    return observer_ptr<VOID>(arg);
  }
  template <typename FROM>
  auto operator()(FROM& from) {
    return lifetime_handle<observer_ptr<VOID>>(from);
  }  // for migration to lifteime_handle! delete after migration!
  template <typename FROM>
  auto operator()(const FROM& from)
    requires observer_ptr<VOID>::is_const
  {
    return lifetime_handle<observer_ptr<VOID>>(from);
  }  // for migration to lifteime_handle! delete after migration!
};

}  // namespace virtual_void::erased

namespace virtual_void::erased {

template <typename VOID>
using observer = lifetime_handle<observer_ptr<VOID>>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

template <typename V, typename VOID>
using typed_observer = typed_lifetime_handle<V, observer_ptr<VOID>>;
template <typename V>
using typed_const_observer = typed_observer<V, void const*>;
template <typename V>
using typed_mutable_observer = typed_observer<V, void*>;

static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

static_assert(is_erased_lifetime_holder<const_observer>);
static_assert(is_erased_lifetime_holder<mutable_observer>);
static_assert(is_erased_lifetime_holder<mutable_observer>);
static_assert(is_erased_lifetime_holder<const_observer>);
static_assert(is_erased_lifetime_holder<typed_const_observer<int>>);
static_assert(is_erased_lifetime_holder<typed_mutable_observer<int>>);

}  // namespace virtual_void::erased