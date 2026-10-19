#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/any_get_meta_data.hpp>
#include <bit_factory/anyxx/any_type_of.hpp>
#include <bit_factory/anyxx/any_cast.hpp>

namespace anyxx {

template <typename V, is_any Any>
struct typed_any : public Any {
  using any_t = Any;
  using proxy_t = typename any_t::proxy_t;
  using proxy_trait_t = any_t::proxy_trait_t;
  using void_t = proxy_trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using any_t::any_t;

  // cppcheck-suppress-begin noExplicitConstructor
  explicit(false) typed_any(V const& v) : any_t(v) {}        // NOLINT
  explicit(false) typed_any(V&& v) : any_t(std::move(v)) {}  // NOLINT
  explicit(false) typed_any(any_t i) : any_t(i) {            // NOLINT
    check_type_match<V>(get_v_table(*this));
  }
  // cppcheck-suppress-end noExplicitConstructor

  value_t const& operator*() const {
    return unchecked_any_cast<value_t const>(*this);
  }
  value_t const* operator->() const {
    return &any_cast<value_t const>(*this);
  }
  value_t const* get() const {
    return &any_cast<value_t const>(*this);
  }
  value_t& operator*() const
    requires(!is_const)
  {
    return any_cast<value_t>(*this);
  }
  value_t* operator->() const
    requires(!is_const)
  {
    return &any_cast<value_t>(*this);
  }
  value_t* get() const
    requires(!is_const)
  {
    return &any_cast<value_t>(*this);
  }
  explicit operator bool() const { return static_cast<bool>(this->proxy_); }
};

template <typename V, is_any Any>
auto as(Any source) {
  return typed_any<V, Any>{std::move(source)};
}

template <typename To, typename V, is_any Any>
auto as(typed_any<V, Any> source)
  requires std::convertible_to<V*, To*>
{
  if constexpr (typed_any<V, Any>::is_const) {
    return typed_any<To const, Any>{std::move(source.proxy_)};
  } else {
    return typed_any<To, Any>{std::move(source.proxy_)};
  }
}

}  // namespace anyxx