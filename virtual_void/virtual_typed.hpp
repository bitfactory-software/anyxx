#pragma once

#include <virtual_void/any_base.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

template <typename V, is_any ANY>
struct virtual_typed : public ANY {
  using erased_data_t = ANY::erased_data_t;
  using trait_t = ANY::trait_t;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using ANY::ANY;

  virtual_typed(V const& v) : ANY(v) {}
  virtual_typed(V&& v) : ANY(std::move(v)) {}
  virtual_typed(ANY i) : ANY(i) {
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
bool has_data(virtual_typed<V, ANY> const& vv) {
  return has_data(vv.erased_data_);
}
template <typename V, is_any ANY>
void const* get_void_data_ptr(virtual_typed<V, ANY> const& vv)
  requires is_const_void<typename ANY::void_t>
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any ANY>
void* get_void_data_ptr(virtual_typed<V, ANY> const& vv)
  requires(!is_const_void<typename ANY::void_t>)
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any ANY>
auto get_meta(virtual_typed<V, ANY> const& vv) {
  return ANY::trait_t::meta(vv.erased_data_);
}

template <typename V, is_any ANY>
auto as(ANY source) {
  return virtual_typed<V, ANY>{std::move(source)};
}

template <typename TO, typename FROM, is_any ANY>
auto as(virtual_typed<FROM, ANY> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (virtual_typed<FROM, ANY>::is_const) {
    return virtual_typed<TO const, ANY>{std::move(source.erased_data_)};
  } else {
    return virtual_typed<TO, ANY>{std::move(source.erased_data_)};
  }
}

}  // namespace virtual_void