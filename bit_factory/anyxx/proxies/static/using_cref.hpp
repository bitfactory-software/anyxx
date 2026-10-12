#pragma once

#include <bit_factory/anyxx/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxy_concept.hpp>

namespace anyxx {

/// Proxy to capture the dispatch target concrete via const& to enable static
/// dispatch A simple wrapper class over an object. Use \c '&' and \c 'const &'
/// to capture by reference.
///
/// Usage:
/// * Use the model map as a static customization point.
///
/// \tparam Value The captured value
template <typename Value>
struct using_cref {
  using_cref() = delete;
  template <typename V>
    requires(!std::same_as<std::decay_t<V>, using_cref>)
  using_cref(V const& v) : value_(v) {}
  Value const& value_;
  operator Value const&() const { return value_; }
};

template <typename V>
struct proxy_trait<using_cref<V>> : basic_proxy_trait<using_cref<V>> {
  static constexpr bool allow_any_default_constructibile = true;

  template <typename Rep>
  using proxy_impl = using_cref<Rep>;
  using void_t = std::conditional_t<std::is_const_v<std::remove_reference_t<V>>,
                                    const_void, mutable_void>;
  using static_dispatch_t = V;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = std::is_constructible_v<
        V, ConstructedWith>;  // && !is_any<ConstructedWith>;
  };
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val,
                               [[maybe_unused]] observeable_v_table* v_table) {
    return val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  static auto construct_in_place(V&& v) { return std::move(v); }
  template <typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return V{std::forward<Args>(args)...};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return using_cref<V>{std::forward<Vx>(v)};
  }
};

}  // namespace anyxx