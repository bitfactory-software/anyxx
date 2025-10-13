#pragma once

#include <utility>
#include <anypp/trait.hpp>

namespace anypp {

struct value_v_table {
  using destroy_fn = void(void*) noexcept;
  using copy_fn = void*(const void*);
  template <class DATA>
  constexpr value_v_table(std::in_place_type_t<DATA>)
      : destroy(&destroy_impl<DATA>), copy(&copy_impl<DATA>) {}
  template <class DATA>
  static void destroy_impl(void* target) noexcept {
    ::delete static_cast<DATA*>(target);
  }
  template <class DATA>
  static void* copy_impl(void const* source) {
    return ::new DATA(*static_cast<DATA const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class DATA>
constexpr value_v_table value_v_table_of =
    value_v_table(std::in_place_type<DATA>);

class value {
 public:
  value() = default;
  template <typename DATA>
  value(DATA* ptr)
      : ptr_(ptr), v_table_(&value_v_table_of<std::decay_t<DATA>>) {}
  value(value const& rhs)
      : ptr_(rhs.ptr_ ? rhs.v_table_->copy(rhs.ptr_) : nullptr),
        v_table_(rhs.v_table_) {}
  value& operator=(const value& rhs) {
    value clone{rhs};
    swap(*this, clone);
    return *this;
  }
  value(value&& rhs) noexcept { swap(*this, rhs); }
  value& operator=(value&& rhs) noexcept {
    value destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  ~value() {
    if (v_table_) v_table_->destroy(ptr_);
  }
  void* get() const { return ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

  friend void swap(value& lhs, value& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(lhs.v_table_, rhs.v_table_);
  }

 private:
  void* ptr_ = nullptr;
  const value_v_table* v_table_ = nullptr;
};

template <typename T, typename... ARGS>
auto make_value(ARGS&&... args) {
  return value(new T(std::forward<ARGS>(args)...));
}

template <typename T, typename... ARGS>
auto make_void_value(ARGS&&... args) {
  return make_value<T>(std::forward<ARGS>(args)...);
}

template <typename T>
auto make_void_value(T&& v) {
  return make_value<T>(std::forward<T>(v));
}

template <typename U>
U* unchecked_unerase_cast(value& v) {
  return static_cast<U*>(v.get());
}
template <typename U>
U const* unchecked_unerase_cast(value const& v) {
  return static_cast<U const*>(v.get());
}

template <>
struct trait<value> {
  using void_t = void*;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_owner = true;
  static constexpr bool is_weak = false;

  static void* value(const auto& v) { return v.get(); }
  static bool has_value(const auto& v) { return v; }

  template <typename CONSTRUCTED_WITH>
  using unerased = CONSTRUCTED_WITH;

  template <typename CONSTRUCTED_WITH>
  static auto erase(CONSTRUCTED_WITH&& v) {
    return anypp::make_value<std::decay_t<CONSTRUCTED_WITH>>(
        std::forward<CONSTRUCTED_WITH>(v));
  }
};

static_assert(is_erased_data<value>);

}  // namespace anypp