#pragma once

#include <anypp/any_base.hpp>
#include <anypp/virtual_void.hpp>

namespace anypp {

template <typename V, is_any ANY>
struct typed_any : public ANY {
  using erased_data_t = ANY::erased_data_t;
  using trait_t = ANY::trait_t;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using ANY::ANY;

  typed_any(V const& v) : ANY(v) {}
  typed_any(V&& v) : ANY(std::move(v)) {}
  typed_any(ANY i) : ANY(i) {
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

template <typename V, is_any ANY>
bool has_data(typed_any<V, ANY> const& vv) {
  return has_data(vv.erased_data_);
}
template <typename V, is_any ANY>
void const* get_void_data_ptr(typed_any<V, ANY> const& vv)
  requires is_const_void<typename ANY::void_t>
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any ANY>
void* get_void_data_ptr(typed_any<V, ANY> const& vv)
  requires(!is_const_void<typename ANY::void_t>)
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any ANY>
auto get_meta(typed_any<V, ANY> const& vv) {
  return ANY::trait_t::meta(vv.erased_data_);
}

template <typename V, is_any ANY>
auto as(ANY source) {
  return typed_any<V, ANY>{std::move(source)};
}

template <typename TO, typename FROM, is_any ANY>
auto as(typed_any<FROM, ANY> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (typed_any<FROM, ANY>::is_const) {
    return typed_any<TO const, ANY>{std::move(source.erased_data_)};
  } else {
    return typed_any<TO, ANY>{std::move(source.erased_data_)};
  }
}

}  // namespace anypp