#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

/// Proxy to manage the captured object via \c std::weak_ptr.
/// Assign or copy construct it from a  \c any_<shared>
/// \ingroup proxies
using weak = std::weak_ptr<void const>;

template <>
struct proxy_trait<weak> : basic_proxy_trait<weak> {
  using void_t = void const*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = false;
  static constexpr bool is_weak =  // NOLINT([duplInheritedMember)
      true;                        // cppcheck-suppress duplInheritedMember
  static constexpr bool allow_any_default_constructibile = true;

  static auto clone_from([[maybe_unused]] const_void data_ptr,  // NOLINT
                         [[maybe_unused]] auto* v_table) {
    return weak{};
  }

  static void const* get_proxy_ptr_in([[maybe_unused]] const auto& ptr,
                                      [[maybe_unused]] auto* v_table) {
    return nullptr;
  }

  template <typename ConstructedWith>
  using unerased = std::decay_t<typename ConstructedWith::element_type>;

  template <typename V>
  static auto construct_in_place(V&&) {
    static_assert(false);
    return nullptr;
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    static_assert(false);
    return nullptr;
  }
  template <typename V>
  static auto erase(std::shared_ptr<V> const& v) {
    return weak{static_pointer_cast<void const>(v)};
  }
  template <typename V>
  static auto erase(std::weak_ptr<V> const& v) {
    return erase(v.lock());
  }
};

static_assert(is_proxy<weak>);

}  // namespace anyxx