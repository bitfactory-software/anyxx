#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/data/has_type_info.h"
#include "../../erased/lifetime/observer.h"

namespace virtual_void::typeid_ {

template <typename VOID>
using observer = erased::observer<VOID, erased::data::has_type_info>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;
static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

template <typename T>
using typed_observer = erased::typed_observer<T, erased::data::has_type_info>;   

struct make_mutable_observer {
  template <typename FROM>
  mutable_observer operator()(FROM&& from) {
    return typed_observer<std::remove_const_t<std::remove_reference_t<FROM> > >(
        std::forward<FROM>(from));
  }
};
struct make_const_observer {
  template <typename FROM>
  const_observer operator()(FROM&& from) {
    return typed_observer<std::add_const_t<std::remove_reference_t<FROM> > >(
        from);
  }
};

static_assert(erased::is_erased_lifetime_holder<mutable_observer>);
static_assert(erased::is_erased_lifetime_holder<const_observer>);
static_assert(erased::is_erased_lifetime_holder<typed_observer<int const> >);
static_assert(erased::is_erased_lifetime_holder<typed_observer<int> >);

}  // namespace virtual_void::m_table