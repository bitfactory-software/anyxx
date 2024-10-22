#pragma once

#include <stdexcept>
#include <type_traits>

#include "../data/has_no_meta/has_no_meta.h"
#include "../data/value_ptr.h"
#include "../lifetime_handle.h"

namespace virtual_void::erased {

using value_data_ptr = data::value_ptr<data::with_no_meta>;

template <>
struct data_trait<value_data_ptr> : data_trait_base<value_data_ptr> {
  using void_t = void*;
  template <typename V>
  using typed_t = data::typed<V, data::has_no_meta>;
  static void* value(const auto& ptr) { return ptr.value(); }
  static auto meta(const auto& ptr) { ptr.meta(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  template <typename V>
  static value_data_ptr construct_from(V&& v) {
    return data::make_value<typed_t<std::decay_t<V>>>(std::forward<V>(v));
  }
  template <typename V, typename... ARGS>
  static value_data_ptr construct_in_place(std::in_place_type_t<V>,
                                           ARGS&&... args) {
    return data::make_value<typed_t<V>>(std::forward<ARGS>(args)...);
  }
};

using value = lifetime_handle<value_data_ptr>;
template <typename T>
using typed_value = typed_lifetime_handle<T, value_data_ptr>;

static_assert(erased::is_erased_lifetime_holder<value>);
static_assert(erased::is_erased_lifetime_holder<typed_value<int>>);

}  // namespace virtual_void::erased