#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

/// Proxy to manage the captured object via \c std::shared_ptr.
/// * If you pass a \c std::shared_ptr to the \ref any constructor, this
/// pointer will be casted to \c <const void*> and used as proxy.
/// * If you pass an object as second parameter, with the std::in_place tag as
/// first, this object will be forwarded to std::make_shared with the decayed
/// type of object.
/// * If you pass as first parameter std::in_place_type<T>, the other
/// arguments will be forwarded to std::make_shared<T>(...).
/// \ingroup proxies
using shared = std::shared_ptr<void const>;

template <>
struct proxy_trait<shared> : basic_proxy_trait<shared> {
  using void_t = void const*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = true;
  static auto clone_from(const_void data_ptr, is_delete_v_table auto* v_table) {
    return shared{copy_construct(v_table, data_ptr),
                  [v_table](auto p) { v_table->delete_(p); }};
  }
  static void move_to(shared& to, [[maybe_unused]] auto v_table_to,
                      shared&& from, [[maybe_unused]] auto) {
    to = std::move(from);
  }
  static void move_to(shared& to, [[maybe_unused]] auto v_table_to, unique from,
                      is_delete_v_table auto* v_table) {
    mutable_void p = nullptr;
    std::swap(from.ptr, p);
    to = shared{p, [v_table](auto px) { v_table->delete_(px); }};
  }

  static void const* get_proxy_ptr_in(const auto& v,
                                      [[maybe_unused]] auto* v_table) {
    return v.get();
  }

  template <typename ConstructedWith>
  struct unerased_impl {
    using type = std::decay_t<ConstructedWith>;
  };
  template <typename V>
  struct unerased_impl<std::shared_ptr<V>> {
    using type = std::decay_t<V>;
  };
  template <typename ConstructedWith>
  using unerased = unerased_impl<ConstructedWith>::type;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return std::make_shared<V>(std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
  template <typename V>
  static auto erase(std::shared_ptr<V> const& v) {
    return static_pointer_cast<void const>(v);
  }
  template <typename V>
    requires(!requires { typename std::decay_t<V>::weak_type; })
  static auto erase(V&& v) {
    return std::make_shared<std::decay_t<V>>(std::forward<V>(v));
  }
};

static_assert(is_proxy<shared>);

}  // namespace anyxx