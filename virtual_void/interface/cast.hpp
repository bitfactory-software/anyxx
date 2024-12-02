#pragma once

#undef interface 

#include <virtual_void/utillities/VV_EXPORT.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/open_method/via_type_info/factory.hpp>
#include <virtual_void/interface/base.hpp>

namespace virtual_void::interface {

 VV_EXPORT inline virtual_void::open_method::via_type_info::domain cast_domain;
 
inline void seal_casts() { seal_for_runtime(cast_domain); }

template <is_virtual_void TO, is_virtual_void FROM>
using copy_factory_method =
    virtual_void::open_method::via_type_info::factory<base<TO>(const FROM&)>;

template <typename CLASS, is_virtual_void TO, is_virtual_void FROM>
copy_factory_method<TO, FROM> copy{cast_domain};

template <is_virtual_void TO, is_virtual_void FROM>
using find_copy_factory_method =
    virtual_void::open_method::via_type_info::factory<
        copy_factory_method<TO, FROM> const&()>;

template <is_virtual_void TO, is_virtual_void FROM>
find_copy_factory_method<TO, FROM> find_copy{cast_domain};

template <is_virtual_void TO, is_virtual_void FROM>
using move_factory_method =
    virtual_void::open_method::via_type_info::factory<base<TO>(FROM&&)>;

template <typename CLASS, is_virtual_void TO, is_virtual_void FROM>
move_factory_method<TO, FROM> move{cast_domain};

template <is_virtual_void TO, is_virtual_void FROM>
using find_move_factory_method =
    virtual_void::open_method::via_type_info::factory<
        move_factory_method<TO, FROM> const&()>;

template <is_virtual_void TO, is_virtual_void FROM>
find_move_factory_method<TO, FROM> find_move{cast_domain};

template <template <is_virtual_void> typename INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM>
auto default_copy() {
  return +[](FROM const& from) -> base<TO> {
    return INTERFACE<TO>{*unchecked_unerase_cast<CLASS>(from)};
  };
}
template <template <is_virtual_void> typename INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM>
auto default_copy()
  requires std::copy_constructible<FROM> && std::same_as<TO, FROM>
{
  return +[](FROM const& from) -> base<TO> {
    typename INTERFACE<TO>::v_table_t* v_table =
        &INTERFACE<TO>::template imlpemented_v_table<CLASS>;
    return INTERFACE<TO>(from, v_table);
  };
};

template <template <is_virtual_void> typename TO_INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM = TO>
void enable_copy_cast(auto impl) {
  using target_interface_t = TO_INTERFACE<TO>;
  copy<CLASS, TO, FROM>.define<target_interface_t>(impl);
  if (!find_copy<TO, FROM>.is_defined<CLASS>())
    find_copy<TO, FROM>.define<CLASS>(
        +[]() -> auto const& { return copy<CLASS, TO, FROM>; });
};

template <template <is_virtual_void> typename TO_INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM = TO>
void enable_copy_cast() {
  enable_copy_cast<TO_INTERFACE, CLASS, TO, FROM>(
      default_copy<TO_INTERFACE, CLASS, TO, FROM>());
}

template <template <is_virtual_void> typename INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM = TO>
auto default_move() {
  return +[](FROM&& from) -> base<TO> {
    typename INTERFACE<TO>::v_table_t* v_table =
        &INTERFACE<TO>::template imlpemented_v_table<CLASS>;
    return base<TO>(std::move(from), v_table);
  };
}

template <template <is_virtual_void> typename TO_INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM = TO>
void enable_move_cast(auto impl) {
  using target_interface_t = TO_INTERFACE<TO>;
  move<CLASS, TO, FROM>.define<target_interface_t>(impl);
  if (!find_move<TO, FROM>.is_defined<CLASS>())
    find_move<TO, FROM>.define<CLASS>(
        +[]() -> auto const& { return move<CLASS, TO, FROM>; });
};

template <template <is_virtual_void> typename TO_INTERFACE, typename CLASS,
          is_virtual_void TO, is_virtual_void FROM = TO>
void enable_move_cast() {
  enable_move_cast<TO_INTERFACE, CLASS, TO, FROM>(
      default_move<TO_INTERFACE, CLASS, TO, FROM>());
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE copy_cast(const FROM_INTERFACE& from_interface) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  using vv_from_t = typename FROM_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  static_assert(is_virtual_void<vv_from_t>);
  const auto& vv_from = get_virtual_void(from_interface);
  auto const& type_info = *get_meta(vv_from)->type_info();
  auto const& copy = find_copy<vv_to_t, vv_from_t>(type_info);
  base<vv_to_t> b = copy.construct<TO_INTERFACE>(vv_from);
  return std::move(unchecked_v_table_cast<TO_INTERFACE>(std::move(b)));
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE move_cast(FROM_INTERFACE&& from_interface) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  using vv_from_t = typename FROM_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  static_assert(is_virtual_void<vv_from_t>);
  auto vv_from = move_virtual_void(std::move(from_interface));
  auto const& type_info = *get_meta(vv_from)->type_info();
  auto const& move = find_move<vv_to_t, vv_from_t>(type_info);
  base<vv_to_t> b = move.construct<TO_INTERFACE>(std::move(vv_from));
  return std::move(unchecked_v_table_cast<TO_INTERFACE>(std::move(b)));
}

};  // namespace virtual_void::interface