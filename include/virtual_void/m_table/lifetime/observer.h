#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/lifetime/observer.h"
#include "../../forward.h"

namespace virtual_void::m_table {

struct make_mutable_observer;
struct make_const_observer;

template <bool>
struct select_make_observer {
  using type = make_mutable_observer;
};
template <>
struct select_make_observer<true> {
  using type = make_const_observer;
};
template <bool is_const>
using select_make_observer_t = select_make_observer<is_const>::type;

template <typename VOID>
struct observer : erased::observer<VOID> {
using base_t = erased::observer<VOID>;
  using base_t::void_t;
  using base_t::is_const;
  using make_erased = select_make_observer_t<is_const>;
  using base_t::base_t;
  template <typename T>
  observer(T&& v)
    requires(!std::derived_from<T, observer>)
      : base_t(v), m_table_( m_table_of<std::remove_reference_t<T>>() ){}
  observer(void_t v, const m_table_t* m_table) : base_t(v), m_table_(m_table) {}
  const m_table_t* m_table_;
  const m_table_t* m_table() const { return m_table_; };
};
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;
static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

template <typename T>
struct select_observer {
  using type = mutable_observer;
};
template <typename T>
struct select_observer<T const> {
  using type = const_observer;
};
template <typename T>
using select_observer_t = select_observer<T>::type;

template <typename T>
struct typed_observer : public select_observer_t<std::remove_reference_t<T> > {
  using conrete_t = std::remove_reference_t<T>;
  using observer_t = select_observer_t<T>;
  typed_observer(const typed_observer&) = default;
  typed_observer(typed_observer&) = default;
  typed_observer(typed_observer&&) = default;
  typed_observer(const observer_t& o)
      : select_observer_t<std::remove_reference_t<T> >(o) {}
  typed_observer(T&& v)
      : select_observer<std::remove_reference_t<T> >(std::forward<T>(v)) {}
  conrete_t& operator*() const {
    return *static_cast<conrete_t*>(this->data());
  }
  conrete_t* operator->() const {
    return static_cast<conrete_t*>(this->data());
  }
};

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

}  // namespace virtual_void::erased