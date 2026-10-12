#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/proxies/dyn/cow.hpp>
#include <bit_factory/anyxx/proxies/dyn/observer.hpp>
#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/weak.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>

namespace anyxx {

template <is_proxy To, is_proxy From>
struct borrow_trait;

template <typename To, typename From, typename FromVTable>
concept borrowable_from =
    is_proxy<From> && is_proxy<To> && requires(From f, FromVTable* v_table) {
      { borrow_trait<To, From>{}(f, v_table) } -> std::same_as<To>;
    };

template <typename To, typename From, typename FromVTable>
  requires borrowable_from<To, From, FromVTable>
To borrow_proxy_as(From const& from, FromVTable* v_table) {
  return borrow_trait<To, From>{}(from, v_table);
}

template <is_proxy From>
  requires(!is_const_data<From> && !is_weak_data<From>)
struct borrow_trait<mutref, From> {
  auto operator()(const auto& from, auto* v_table) const {
    return mutref{get_proxy_ptr(from, v_table)};
  }
};
template <is_proxy From>
  requires(!is_weak_data<From>)
struct borrow_trait<cref, From> {
  auto operator()(const auto& from, [[maybe_unused]] auto* v_table) const {
    return cref{get_proxy_ptr(from, v_table)};
  }
};
template <>
struct borrow_trait<shared, shared> {
  auto operator()(const auto& from, [[maybe_unused]] auto* v_table) const {
    return from;
  }
};
template <>
struct borrow_trait<weak, weak> {
  auto operator()(const auto& from, [[maybe_unused]] auto* v_table) const {
    return from;
  }
};
template <>
struct borrow_trait<weak, shared> {
  auto operator()(const auto& from, [[maybe_unused]] auto* v_table) const {
    return weak{from};
  }
};

}  // namespace anyxx
