#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <typename DATA>
struct trait;

template <class E>
concept is_erased_data = requires(E e) {
  typename trait<E>::void_t;
  { trait<E>::is_constructibile_from_const } -> std::convertible_to<bool>;
  { trait<E>::value(e) } -> std::convertible_to<typename trait<E>::void_t>;
  { trait<E>::has_value(e) } -> std::convertible_to<bool>;
};

template <is_erased_data DATA>
using data_void = trait<DATA>::void_t;

template <typename ERASED_DATA>
concept is_const_data =
    is_erased_data<ERASED_DATA> && is_const_void<data_void<ERASED_DATA>>;

template <bool CALL_IS_CONST, bool ERASED_DATA_IS_CONST>
concept const_correct_call =
    ((CALL_IS_CONST == ERASED_DATA_IS_CONST) || !ERASED_DATA_IS_CONST);

template <typename CALL, typename ERASED_DATA>
concept const_correct_call_for_erased_data =
    is_ereasurness<CALL> && is_erased_data<ERASED_DATA> &&
    (const_correct_call<is_const_void<CALL>, is_const_data<ERASED_DATA>>);

template <is_erased_data ERASED_DATA, typename FROM>
ERASED_DATA erased(FROM&& from) {
  return trait<ERASED_DATA>::erase(std::forward<FROM>(from));
}

template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH>
using unerased =
    trait<ERASED_DATA>::template unerased<std::decay_t<CONSTRUCTED_WITH>>;

template <is_erased_data ERASED_DATA>
bool has_data(ERASED_DATA const& vv) {
  return trait<ERASED_DATA>::has_value(vv);
}
template <is_erased_data ERASED_DATA>
void const* get_data(ERASED_DATA const& vv)
  requires std::same_as<void const*, typename trait<ERASED_DATA>::void_t>
{
  return trait<ERASED_DATA>::value(vv);
}
template <is_erased_data ERASED_DATA>
void* get_data(ERASED_DATA const& vv)
  requires std::same_as<void*, typename trait<ERASED_DATA>::void_t>
{
  return trait<ERASED_DATA>::value(vv);
}

template <typename U>
auto unchecked_unerase_cast(void const* p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(void* p) {
  return static_cast<U*>(p);
}

template <typename U, is_erased_data ERASED_DATA>
auto unchecked_unerase_cast(ERASED_DATA const& o) {
  return unchecked_unerase_cast<U>(get_data(o));
}
template <typename U, is_erased_data ERASED_DATA>
auto unchecked_unerase_cast(ERASED_DATA const& o)
  requires(!is_const_data<ERASED_DATA>)
{
  return unchecked_unerase_cast<U>(get_data(o));
}

template <typename U, is_erased_data ERASED_DATA>
auto unerase_cast(ERASED_DATA const& o, runtime::meta_data const& meta) {
  check_type_match<U>(meta);
  return unchecked_unerase_cast<U>(o);
}
template <typename U, is_erased_data ERASED_DATA>
U const* unerase_cast(ERASED_DATA const* o, runtime::meta_data const& meta) {
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}
template <typename U, is_erased_data ERASED_DATA>
U* unerase_cast(ERASED_DATA const* o, runtime::meta_data const& meta)
  requires(!is_const_data<ERASED_DATA>)
{
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}

template <typename CONSTRUCTED_WITH, typename ERASED_DATA, typename BASE>
concept erased_constructibile_for =
    !std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, BASE> &&
    !is_erased_data<std::remove_cvref_t<CONSTRUCTED_WITH>> &&
    (!std::is_const_v<std::remove_reference_t<CONSTRUCTED_WITH>> ||
     trait<ERASED_DATA>::is_constructibile_from_const);

}  // namespace virtual_void::data