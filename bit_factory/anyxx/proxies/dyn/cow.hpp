#pragma once

#include <atomic>
#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>
#include <bit_factory/anyxx/voidness.hpp>
#include <cassert>
#include <cstddef>
#include <utility>

namespace anyxx {

/// \brief Proxy to manage the captured object as value via copy-on-write
/// * If you forward an object to any constructor, this object will be
/// forwarded to the allocated storage.
/// * To pass an object as second parameter, with the std::in_place tag as
/// first, has the same behavior as above
/// * If you pass as first parameter std::in_place_type<...>, the object will
/// be constructed in place in the allocated memory with the other arguments
/// forwarded
/// \ingroup proxies
struct cow {
  struct holder_base {
    std::atomic<std::size_t> count_{1};
  };
  template <typename T = void*>
  struct holder : holder_base {
    alignas(std::nullptr_t) T value_;

    holder() noexcept(std::is_nothrow_constructible_v<T>) = default;

    template <class... Args>
    explicit holder(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args&&...>)
        : value_(std::forward<Args>(args)...) {}
  };
  holder_base* holder_ = nullptr;

  inline static size_t constexpr offset_of_value() {
    cow::holder<> object{};
    return size_t(&(object.value_)) - size_t(&object);
  }

  [[nodiscard]] static holder_base* holder_from_data_ptr(
      mutable_void data_ptr) {
    return static_cast<cow::holder<>*>(static_cast<mutable_void>(
        static_cast<std::byte*>(data_ptr) - offset_of_value()));
  }
  [[nodiscard]] static void* data_ptr_from_holder(holder_base* holder) {
    return &static_cast<cow::holder<>*>(holder)->value_;
  }
  [[nodiscard]] void* data_ptr() const { return data_ptr_from_holder(holder_); }
  [[nodiscard]] auto unique() const noexcept -> bool {
    assert(holder_ && "FATAL (sparent) : using a moved copy_on_write object");
    return holder_->count_.load(std::memory_order_acquire) == 1;
  }

  cow(holder_base* h = nullptr) : holder_(h) {}
  template <typename T, typename... Args>
  cow(std::in_place_type_t<T>, Args&&... args)
      : holder_(new holder<T>(std::forward<Args>(args)...)) {}
  cow(cow const&) : holder_(nullptr) {}
  cow& operator=(cow const&) { return *this; }
  cow& operator=(cow&&) { return *this; }
  void emplace(cow&& other) {
    assert(holder_ == nullptr);
    holder_ = other.holder_;
    other.holder_ = nullptr;
  }
  ~cow() {}
};

template <>
struct proxy_trait<cow> : basic_proxy_trait<cow> {
  using void_t = void*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = true;
  static constexpr bool allow_any_default_constructibile = true;

  template <typename VTable>
  static constexpr bool is_compatible_with_v_table() {
    return is_copy_constructor_v_table<VTable> &&
           is_destructor_v_table<VTable> && is_model_size_v_table<VTable>;
  }

  static cow clone_from([[maybe_unused]] mutable_void data_ptr,
                        [[maybe_unused]] auto* v_table) {
    auto clone = cow::holder_from_data_ptr(data_ptr);
    clone->count_.fetch_add(1, std::memory_order_relaxed);
    return {clone};
  }
  static void move_to(cow& to, [[maybe_unused]] std::nullptr_t v_table_to,
                      cow&& from, [[maybe_unused]] auto v_table_from) {
    to.holder_ = std::exchange(from.holder_, nullptr);
  }
  static void move_to(cow& to, auto* v_table_to, cow&& from,
                      [[maybe_unused]] auto* v_table_from) {
    destroy(to, v_table_to);
    move_to(to, nullptr, std::move(from), nullptr);
  }
  static void assign(cow& to, cow const& from) {
    to.holder_ = from.holder_;
    to.holder_->count_.fetch_add(1, std::memory_order_relaxed);
  }
  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return is_copy_constructor_v_table<VTable>;
  }
  static void copy_construct_from(cow& to, [[maybe_unused]] auto v_table_to,
                                  cow const& from,
                                  [[maybe_unused]] auto* v_table_from) {
    destroy(to, v_table_to);
    assign(to, from);
  }
  static void destroy(cow& v, auto v_table) {
    if (v.holder_ && v_table != nullptr &&
        (v.holder_->count_.fetch_sub(1, std::memory_order_release) == 1)) {
      std::atomic_thread_fence(std::memory_order_acquire);
      destruct(v_table, v.data_ptr());
      deallocate(v_table, v.holder_);
    }
    v.holder_ = nullptr;
  }

  static void* get_proxy_ptr_in(cow const& v, [[maybe_unused]] auto* v_table) {
    return v.data_ptr();
  }
  template <typename VTable>
  static void* get_proxy_ptr_in(cow& v, VTable* v_table) {
    if (!v.unique()) {
      if constexpr (is_copy_constructor_v_table<VTable>) {
        auto holder_size = sizeof(cow::holder<>) - sizeof(void*) +
                           get_model_size(v_table).size;
        auto holder =
            new (static_cast<cow::holder_base*>(::operator new(holder_size)))
                cow::holder_base;
        copy_construct_at(v_table, cow::data_ptr_from_holder(holder),
                          cow::data_ptr_from_holder(v.holder_));
        destroy(v, v_table);
        v.holder_ = holder;
      } else {
        assert(false && "FATAL: movable only v-table.");
      }
    }
    return v.data_ptr();
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return cow(std::in_place_type<std::decay_t<V>>, std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return cow(std::in_place_type<T>, std::forward<Args>(args)...);
  }
  template <typename ConstructedWith>
  static auto erase(ConstructedWith&& v) {
    return cow(std::in_place_type<std::decay_t<ConstructedWith>>,
               std::forward<ConstructedWith>(v));
  }
};

static_assert(is_proxy<cow>);
static_assert(is_object_proxy<cow>);

}  // namespace anyxx