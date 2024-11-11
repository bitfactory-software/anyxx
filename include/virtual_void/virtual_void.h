#pragma once

#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

namespace virtual_void {

using type_info_ptr = std::type_info const*;

template <typename>
struct self_pointer;
template <>
struct self_pointer<void*> {
  template <typename CLASS>
  using type = CLASS*;
};
template <>
struct self_pointer<const void*> {
  template <typename CLASS>
  using type = const CLASS*;
};

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

template <class BASE, class DERIVED>
concept base_of = std::derived_from<DERIVED, BASE>;

template <typename DATA>
struct virtual_void_trait;

template <class META>
concept is_meta = requires(META meta) {
  { meta.get_meta() } -> std::convertible_to<META const*>;
  { meta.type_info() } -> std::convertible_to<type_info_ptr>;
};

template <class PTR>
concept has_virtual_void_trait =
    requires(PTR ptr) { typename virtual_void_trait<PTR>::void_t; };

template <class E>
concept is_virtual_void = has_virtual_void_trait<E>;

template <class E>
concept is_virtual_typed = requires(E e) {
  typename E::void_t;
  typename E::virtual_void_t;
  typename E::trait_t;
  //  typename E::make_erased;
  // typename E::trait_t;
  { E::is_const } -> std::convertible_to<bool>;
  //  { E::make_erased()(i) } -> base_of<E>;
  //	{ get_data(e.data_) } -> std::convertible_to<typename E::void_t>;
  //  { e.meta()->type_info() } -> std::convertible_to<std::type_info const*>;
};

struct mutable_ {};
struct const_ {};
template <typename VOID>
struct is_const_void_;
template <>
struct is_const_void_<void*> : std::false_type {};
template <>
struct is_const_void_<void const*> : std::true_type {};
template <>
struct is_const_void_<mutable_> : std::false_type {};
template <>
struct is_const_void_<const_> : std::true_type {};
template <typename CONST_SPECIFIER>
struct void_;
template <>
struct void_<mutable_> {
  using type = void*;
};
template <>
struct void_<const_> {
  using type = void const*;
};
template <typename CONST_SPECIFIER>
using void_t = void_<CONST_SPECIFIER>::type;

template <typename VOID>
concept is_const_void = is_const_void_<VOID>::value;

template <typename DATA>
using data_void = virtual_void_trait<DATA>::void_t;

template <typename DATA>
concept is_const_data = is_const_void<data_void<DATA>>;

template <typename TARGET, typename DATA>
concept const_correct_target_for_data =
    ((is_const_void<TARGET> == is_const_void<DATA>) || (!is_const_void<DATA>));

template <typename VOID, typename DATA>
concept const_correct_for_virtual_void_data =
    const_correct_target_for_data<VOID, data_void<DATA>>;

template <typename VOID, typename VIRTUAL_VOID>
concept const_correct_for_virtual_void =
    (const_correct_for_virtual_void_data<VOID, VIRTUAL_VOID>);

template <typename V, is_virtual_void DATA>
struct virtual_typed;

template <is_virtual_void VIRTUAL_VOID, typename FROM>
VIRTUAL_VOID erased(FROM&& from) {
  return virtual_void_trait<VIRTUAL_VOID>::construct_from(
      std::forward<FROM>(from));
}
template <is_virtual_void VIRTUAL_VOID, typename FROM>
VIRTUAL_VOID erase_to(FROM&& from) {
  if constexpr (is_virtual_void<std::remove_reference_t<FROM>>) {
    return from;
  } else {
    return erased<VIRTUAL_VOID>(from);
  }
}
template <is_virtual_void VIRTUAL_VOID, typename V, typename... ARGS>
VIRTUAL_VOID erased_in_place(ARGS&&... args) {
  return virtual_void_trait<VIRTUAL_VOID>::construct_in_place(
      std::in_place_type<V>, std::forward<ARGS>(args)...);
}

struct virtual_void_default_unerase {
  template <typename CONSTRUCTED_WITH>
  using unerased_type = CONSTRUCTED_WITH;
};

template <typename T>
struct static_cast_uneraser {
  using type = T;
  auto operator()(auto from) { return static_cast<T*>(from); }
};
template <is_virtual_void VIRTUAL_VOID, typename CONSTRUCTED_WITH>
auto unerase() {
  using constructed_with_t = std::remove_cvref_t<CONSTRUCTED_WITH>;
  using trait_t = virtual_void_trait<VIRTUAL_VOID>;
  if constexpr (is_virtual_void<constructed_with_t>) {
    using value_t = typename constructed_with_t::value_t;
    return static_cast_uneraser<value_t>();
  } else {
    using value_t = trait_t::template unerased_type<constructed_with_t>;
    if constexpr (is_const_data<VIRTUAL_VOID>) {
      return static_cast_uneraser<value_t const>();
    } else {
      return static_cast_uneraser<value_t>();
    }
  }
}

template <is_virtual_void VIRTUAL_VOID>
bool has_data(VIRTUAL_VOID const& vv) {
  return virtual_void_trait<VIRTUAL_VOID>::has_value(vv);
}
template <is_virtual_void VIRTUAL_VOID>
void const* get_data(VIRTUAL_VOID const& vv)
  requires std::same_as<void const*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return virtual_void_trait<VIRTUAL_VOID>::value(vv);
}
template <is_virtual_void VIRTUAL_VOID>
void* get_data(VIRTUAL_VOID const& vv)
  requires std::same_as<void*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return virtual_void_trait<VIRTUAL_VOID>::value(vv);
}
template <is_virtual_void VIRTUAL_VOID>
auto get_meta(VIRTUAL_VOID const& vv) {
  return virtual_void_trait<VIRTUAL_VOID>::meta(vv);
}

template <typename U, is_virtual_void VIRTUAL_VOID>
auto unchecked_unerase_cast(VIRTUAL_VOID const& o) {
  return static_cast<U const*>(get_data(o));
}
template <typename U, is_virtual_void VIRTUAL_VOID>
auto unchecked_unerase_cast(VIRTUAL_VOID const& o)
  requires(!is_const_data<VIRTUAL_VOID>)
{
  return static_cast<U*>(get_data(o));
}
template <typename U, typename META>
bool type_match(META const* meta) {
  if (auto type_info = meta->type_info(); type_info && *type_info != typeid(U))
    return false;
  return true;
}
class type_mismatch_error : error {
  using error::error;
};
template <typename U, typename META>
void check_type_match(META const* meta) {
  if (auto type_info = meta->type_info(); type_info && *type_info != typeid(U))
    throw type_mismatch_error("type mismatch");
}
template <typename U, is_virtual_void VIRTUAL_VOID>
void check_type_match(VIRTUAL_VOID const& o) {
  check_type_match<U>(get_meta(o));
}
template <typename U, is_virtual_void VIRTUAL_VOID>
auto unerase_cast(VIRTUAL_VOID const& o) {
  check_type_match<U>(o);
  return unchecked_unerase_cast<U>(o);
}
template <typename U, is_virtual_void VIRTUAL_VOID>
U const* unerase_cast(VIRTUAL_VOID const* o) {
  if (type_match<U>(get_meta(*o))) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}
template <typename U, is_virtual_void VIRTUAL_VOID>
U* unerase_cast(VIRTUAL_VOID const* o)
  requires(!is_const_data<VIRTUAL_VOID>)
{
  if (type_match<U>(get_meta(*o))) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}

template <typename V, is_virtual_void VIRTUAL_VOID>
struct virtual_typed {
  VIRTUAL_VOID virtual_void_;

  using virtual_void_t = VIRTUAL_VOID;
  using trait_t = virtual_void_trait<VIRTUAL_VOID>;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  virtual_typed(const virtual_typed&) = default;
  virtual_typed(virtual_typed&) = default;
  virtual_typed(virtual_typed&&) = default;
  virtual_typed& operator=(const virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&&) = default;
  template <typename FROM>
  explicit virtual_typed(FROM&& from)
    requires(!std::same_as<std::decay_t<FROM>, virtual_typed> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : virtual_void_(trait_t::construct_in_place(std::in_place_type<V>,
                                                  std::forward<FROM>(from))) {
    check_type_match<V>(virtual_void_);
  }
  template <typename... ARGS>
  virtual_typed(std::in_place_t, ARGS&&... args)
      : virtual_void_(trait_t::construct_in_place(
            std::in_place_type<V>, std::forward<ARGS>(args)...)) {
    check_type_match<V>(virtual_void_);
  }

  explicit virtual_typed(VIRTUAL_VOID data) : virtual_void_(std::move(data)) {}

  value_t const& operator*() const {
    return *unchecked_unerase_cast<value_t const>(virtual_void_);
  }
  value_t const* operator->() const {
    return unchecked_unerase_cast<value_t const>(virtual_void_);
  }
  value_t& operator*() const
    requires !is_const
  {
    return *unchecked_unerase_cast<value_t>(virtual_void_);
  }
  value_t* operator->() const
    requires !is_const
  {
    return unchecked_unerase_cast<value_t>(virtual_void_);
  }
};

template <typename V, typename VIRTUAL_VOID>
bool has_data(virtual_typed<V, VIRTUAL_VOID> const& vv) {
  return has_data(vv.virtual_void_);
}
template <typename V, typename VIRTUAL_VOID>
void const* get_data(virtual_typed<V, VIRTUAL_VOID> const& vv)
  requires std::same_as<void const*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return get_data(vv.virtual_void_);
}
template <typename V, typename VIRTUAL_VOID>
void* get_data(virtual_typed<V, VIRTUAL_VOID> const& vv)
  requires std::same_as<void*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return get_data(vv.virtual_void_);
}
template <typename V, typename VIRTUAL_VOID>
auto get_meta(virtual_typed<V, VIRTUAL_VOID> const& vv) {
  return virtual_void_trait<VIRTUAL_VOID>::meta(vv.virtual_void_);
}

template <typename V, is_virtual_void DATA>
auto as(DATA source) {
  return virtual_typed<V, DATA>{std::move(source)};
}

template <typename TO, typename FROM, typename DATA>
auto as(virtual_typed<FROM, DATA> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (virtual_typed<FROM, DATA>::is_const) {
    return virtual_typed<TO const, DATA>{std::move(source.virtual_void_)};
  } else {
    return virtual_typed<TO, DATA>{std::move(source.virtual_void_)};
  }
}

}  // namespace virtual_void