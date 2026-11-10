#pragma once

#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/voidness.hpp>
#include <utility>
#include <cassert>          
#include <memory>          


namespace anyxx {

/// Proxy to manage the captured object via std::unique_ptr-like smart pointer
/// * If you pass a std::unique_ptr to the any_ constructor, this pointer will
/// be released and the ownership goes to the unique. NOTE: The \ref
/// any_v_table is built with the value_type of the std::unique_ptr.
/// * If you pass an object as second parameter, with the std::in_place tag as
/// first, this object will be moved to the memory managed by the unique.
/// * If you pass as first parameter std::in_place_type<...>, the object will
/// be constructed in place in the allocated memory with the other arguments
/// forwarded.
/// \ingroup proxies
struct unique {
  mutable_void ptr = nullptr;
  explicit unique(mutable_void p = nullptr) : ptr(p) {}
  unique(unique const&) = delete;
  unique& operator=(unique const&) = delete;
  unique(unique&& other) noexcept { std::swap(ptr, other.ptr); }
  unique& operator=(unique&& other) noexcept {
    assert(!ptr);
    std::swap(ptr, other.ptr);
    return *this;
  };
  ~unique() = default;
  explicit operator bool() const { return static_cast<bool>(ptr); }
};

template <>
struct proxy_trait<unique> : basic_proxy_trait<unique> {
  using void_t = void*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  template <typename VTable>
  static constexpr bool is_compatible_with_v_table() {
    return is_delete_v_table<VTable>;
  }
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return unique{copy_construct(v_table, data_ptr)};
  }
  static void move_to(unique& to, auto v_table_to, unique&& from,
                      [[maybe_unused]] auto* v_table_from) {
    mutable_void old = nullptr;
    std::swap(to.ptr, old);
    std::swap(to.ptr, from.ptr);
    invoke_delete_(v_table_to, old);
  }

  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return false;
  }

  static void* get_proxy_ptr_in(const auto& ptr,
                                [[maybe_unused]] auto v_table) {
    return ptr.ptr;
  }

  static void destroy(unique& u, is_delete_v_table auto* v_table) {
    assert(v_table || !u.ptr);
    if (v_table) invoke_delete_(v_table, u.ptr);
  }

  template <typename ConstructedWith>
  struct unerased_impl {
    using type = std::decay_t<ConstructedWith>;
  };
  template <typename V>
  struct unerased_impl<std::unique_ptr<V>> {
    using type = std::decay_t<V>;
  };
  template <typename ConstructedWith>
  using unerased = unerased_impl<ConstructedWith>::type;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return unique{new V{std::forward<V>(v)}};
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return unique{new T{std::forward<Args>(args)...}};
  }
  template <typename V>
  static auto erase(std::unique_ptr<V>&& v) {
    return unique{v.release()};
  }
  template <typename V>
    requires(!requires { typename std::decay_t<V>::deleter_type; })
  static auto erase(V&& v) {
    return unique{new V{std::move(v)}};
  }
};

static_assert(is_proxy<unique>);

}  // namespace anyxx