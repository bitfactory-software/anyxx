#pragma once

#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/voidness.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {

template <typename U>
auto unchecked_unerase_cast(const_void p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(mutable_void p) {
  return static_cast<U*>(p);
}

template <typename U, is_proxy Proxy>
auto unchecked_unerase_cast(Proxy const& o, auto v_table) {
  return unchecked_unerase_cast<U>(get_proxy_ptr(o, v_table));
}

template <typename U, is_proxy Proxy>
auto unerase_cast(Proxy const& o, auto* v_table) {
  check_type_match<U>(v_table);
  return unchecked_unerase_cast<U>(o, v_table);
}
template <typename U, is_proxy Proxy>
U const* unerase_cast_if(Proxy const& o, auto* v_table) {
  if (type_match<U>(v_table)) return unchecked_unerase_cast<U>(o, v_table);
  return nullptr;
}
template <typename U, is_proxy Proxy>
U* unerase_cast_if(Proxy const& o, auto* v_table)
  requires(!is_const_data<Proxy>)
{
  if (type_match<U>(v_table)) return unchecked_unerase_cast<U>(o, v_table);
  return nullptr;
}

}  // namespace anyxx