#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/data/has_m_table/has_m_table.h"
#include "../../erased/data/observer_ptr.h"
#include "../../erased/lifetime_handle.h"

namespace virtual_void::m_table {
template <typename VOID>
using observer_ptr = erased::data::observer_ptr<VOID, erased::data::has_m_table>;
}

namespace virtual_void::erased {

template <typename VOID>
struct data_trait<m_table::observer_ptr<VOID>>
    : data_trait_base<m_table::observer_ptr<VOID>> {
  using void_t = VOID;
  static void_t value(const auto& ptr) { return ptr.value(); }
  static auto meta(const auto& ptr) { return ptr.meta(); }
  static bool has_value(const auto& ptr) {
    return static_cast<bool>(ptr.value());
  }
  template <typename V>
  static auto construct_from(V&& v) {
    return m_table::observer_ptr<VOID>(std::forward<V>(v));
  }
  template <typename V, typename... ARGS>
  static auto construct_in_place(std::in_place_type_t<V>,
                                                ARGS&&... args) {
    static_assert(sizeof...(ARGS) == 1);
    return m_table::observer_ptr<VOID>(std::forward<ARGS>(args)...);
  }
};

}  // namespace virtual_void::erased

namespace virtual_void::m_table {

template <typename VOID>
using observer = erased::lifetime_handle<m_table::observer_ptr<VOID>>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

template <typename V, typename VOID>
using typed_observer = erased::typed_lifetime_handle<V, m_table::observer_ptr<VOID>>;
template <typename V>
using typed_const_observer = typed_observer<V, void const*>;
template <typename V>
using typed_mutable_observer = typed_observer<V, void*>;

static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

static_assert(erased::is_erased_lifetime_holder<const_observer>);
static_assert(erased::is_erased_lifetime_holder<mutable_observer>);
static_assert(erased::is_erased_lifetime_holder<mutable_observer>);
static_assert(erased::is_erased_lifetime_holder<const_observer>);
static_assert(erased::is_erased_lifetime_holder<typed_const_observer<int>>);
static_assert(erased::is_erased_lifetime_holder<typed_mutable_observer<int>>);

}  // namespace virtual_void::m_table