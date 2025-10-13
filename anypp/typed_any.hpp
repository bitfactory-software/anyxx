#pragma once

#include <anypp/any_base.hpp>
#include <anypp/anypp.hpp>

namespace anypp {

template <typename V, is_any Any>
struct typed_any : public Any {
  using erased_data_t = Any::erased_data_t;
  using trait_t = Any::trait_t;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using Any::Any;

  typed_any(V const& v) : Any(v) {}
  typed_any(V&& v) : Any(std::move(v)) {}
  typed_any(Any i) : Any(i) {
    check_type_match<V>(get_runtime(*this));
  }

  value_t const& operator*() const {
    return *unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t const* operator->() const {
    return unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t const* get() const {
    return unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t& operator*() const
    requires !is_const
  {
    return *unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* operator->() const
    requires !is_const
  {
    return unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* get() const
    requires !is_const
  {
    return unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  explicit operator bool() const {
    return static_cast<bool>(this->erased_data_);
  }
};

template <typename V, is_any Any>
bool has_data(typed_any<V, Any> const& vv) {
  return has_data(vv.erased_data_);
}
template <typename V, is_any Any>
void const* get_void_data_ptr(typed_any<V, Any> const& vv)
  requires is_const_void<typename Any::void_t>
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any Any>
void* get_void_data_ptr(typed_any<V, Any> const& vv)
  requires(!is_const_void<typename Any::void_t>)
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any Any>
auto get_meta(typed_any<V, Any> const& vv) {
  return Any::trait_t::meta(vv.erased_data_);
}

template <typename V, is_any Any>
auto as(Any source) {
  return typed_any<V, Any>{std::move(source)};
}

template <typename TO, typename FROM, is_any Any>
auto as(typed_any<FROM, Any> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (typed_any<FROM, Any>::is_const) {
    return typed_any<TO const, Any>{std::move(source.erased_data_)};
  } else {
    return typed_any<TO, Any>{std::move(source.erased_data_)};
  }
}

}  // namespace anypp