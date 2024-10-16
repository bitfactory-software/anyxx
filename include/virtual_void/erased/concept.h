#pragma once

namespace virtual_void::erased {

template <class BASE, class DERIVED>
concept base_of = std::derived_from<DERIVED, BASE>;

template <class E>
concept is_erased_lifetime_holder = requires(E e, int i) {
  typename E::void_t;
  { E::is_const } -> std::convertible_to<bool>;
  typename E::make_erased;

  { E::make_erased()(i) } -> base_of<E>;
  { e.data() } -> std::convertible_to<const void*>;
};

template <is_erased_lifetime_holder LIFETIME_HOLDER, typename FROM>
LIFETIME_HOLDER erase_to(FROM&& from) {
  if constexpr (is_erased_lifetime_holder<std::remove_reference_t<FROM> >) {
    return from;
  } else {
    return LIFETIME_HOLDER::make_erased()(std::forward<FROM>(from));
  }
}

template <typename T>
struct unerase_t {
  using type = T;
  auto operator()(auto from) { return static_cast<T*>(from); }
};

template <is_erased_lifetime_holder LIFETIME_HOLDER, typename CONSTRUCTED_WITH>
auto unerase() {
  using constructed_with_t = std::remove_cvref_t<CONSTRUCTED_WITH>;
  if constexpr (is_erased_lifetime_holder<constructed_with_t>) {
    return unerase_t<typename constructed_with_t::conrete_t>();
  } else {
    if constexpr (LIFETIME_HOLDER::is_const) {
      return unerase_t<constructed_with_t const>();
    } else {
      return unerase_t<constructed_with_t>();
    }
  }
}

}  // namespace virtual_void::erased
