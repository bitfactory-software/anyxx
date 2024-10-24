#pragma once

#include <assert.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../../erased/concept.h"
#include "../m_table.h"
#include "data.h"

namespace virtual_void::m_table {

using unique_abstract_data_ptr = std::unique_ptr<abstract_data>;
class unique {
  unique_abstract_data_ptr ptr_;

 protected:
  unique(unique_abstract_data_ptr&& ptr) : ptr_(std::move(ptr)) {}

 public:
  using void_t = void*;
  static constexpr bool is_const = false;
  void* data() const { return ptr_->data_; }
  const std::type_info& type() const { return ptr_->m_table_->type(); }
  const m_table_t* m_table() const { return ptr_->m_table_; };
};
static_assert(MtableDispatchableVoid<const unique, void*>);
static_assert(MtableDispatchableVoid<const unique, const void*>);

template <typename T>
class typed_unique : public unique {
  template <typename T>
  friend auto as(unique&& source);
  typed_unique(unique&& u) noexcept : unique(std::move(u)) {}

 public:
  using conrete_t = T;
  using unique::unique;
  typed_unique(T&& v) noexcept
      : unique(std::make_unique<concrete_data<T> >(std::move(v))) {}
  template <typename... ARGS>
  typed_unique(std::in_place_t, ARGS&&... args) noexcept
      : unique(std::make_unique<concrete_data<T> >(
            std::in_place, std::forward<ARGS>(args)...)) {}
  T& operator*() const { return *static_cast<T*>(data()); }
  T* operator->() const { return static_cast<T*>(data()); }
};
static_assert(MtableDispatchableVoid<const typed_unique<nullptr_t>, void*>);
static_assert(
    MtableDispatchableVoid<const typed_unique<nullptr_t>, const void*>);

template <typename T>
auto as(unique&& source) {
  if (source.type() != typeid(T))
    throw error("source is: " + std::string(source.type().name()) + ".");
  return typed_unique<T>{std::move(source)};
}
template <typename T, typename... ARGS>
typed_unique<T> make_unique(ARGS&&... args) {
  return {std::in_place, std::forward<ARGS>(args)...};
}

}  // namespace virtual_void::m_table