#pragma once

#include <virtual_void/interface/base.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

template <typename V, is_interface INTERFACE>
struct virtual_typed : public INTERFACE {
  using erased_data_t = INTERFACE::erased_data_t;
  using trait_t = INTERFACE::trait_t;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using INTERFACE::INTERFACE;

  virtual_typed(V const& v) : INTERFACE(v) {}
  virtual_typed(V&& v) : INTERFACE(std::move(v)) {}
  virtual_typed(INTERFACE i) : INTERFACE(i) {
    check_type_match<V>(get_runtime(*this));
  }

  value_t const& operator*() const {
    return *data::unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t const* operator->() const {
    return data::unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t const* get() const {
    return data::unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t& operator*() const
    requires !is_const
  {
    return *data::unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* operator->() const
    requires !is_const
  {
    return data::unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* get() const
    requires !is_const
  {
    return data::unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  explicit operator bool() const {
    return static_cast<bool>(this->erased_data_);
  }
};

template <typename V, is_interface INTERFACE>
bool has_data(virtual_typed<V, INTERFACE> const& vv) {
  return has_data(vv.erased_data_);
}
template <typename V, is_interface INTERFACE>
void const* get_void_data_ptr(virtual_typed<V, INTERFACE> const& vv)
  requires is_const_void<typename INTERFACE::void_t>
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_interface INTERFACE>
void* get_void_data_ptr(virtual_typed<V, INTERFACE> const& vv)
  requires(!is_const_void<typename INTERFACE::void_t>)
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_interface INTERFACE>
auto get_meta(virtual_typed<V, INTERFACE> const& vv) {
  return INTERFACE::trait_t::meta(vv.erased_data_);
}

template <typename V, is_interface INTERFACE>
auto as(INTERFACE source) {
  return virtual_typed<V, INTERFACE>{std::move(source)};
}

template <typename TO, typename FROM, is_interface INTERFACE>
auto as(virtual_typed<FROM, INTERFACE> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (virtual_typed<FROM, INTERFACE>::is_const) {
    return virtual_typed<TO const, INTERFACE>{std::move(source.erased_data_)};
  } else {
    return virtual_typed<TO, INTERFACE>{std::move(source.erased_data_)};
  }
}

}  // namespace virtual_void