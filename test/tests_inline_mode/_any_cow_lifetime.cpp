#include <atomic>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

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

  cow(holder_base* holder = nullptr) : holder_(holder) {}
  template <typename T, typename... Args>
  cow(std::in_place_type_t<T>, Args&&... args)
      : holder_(new holder<T>(std::forward<Args>(args)...)) {}
  cow(cow const&) {}
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
                        [[maybe_unused]] is_v_table auto* v_table) {
    auto clone = cow::holder_from_data_ptr(data_ptr);
    clone->count_.fetch_add(1, std::memory_order_relaxed);
    return {clone};
  }
  static void move_to(cow& to, [[maybe_unused]] std::nullptr_t v_table_to,
                      cow&& from, [[maybe_unused]] auto v_table_from) {
    to.holder_ = std::exchange(from.holder_, nullptr);
  }
  static void move_to(cow& to, is_v_table auto* v_table_to, cow&& from,
                      [[maybe_unused]] is_v_table auto* v_table_from) {
    destroy(to, v_table_to);
    move_to(to, nullptr, std::move(from), nullptr);
  }
  static void copy_construct_from(
      cow& to, [[maybe_unused]] auto v_table_to, cow const& from,
      [[maybe_unused]] is_v_table auto* v_table_from) {
    destroy(to, v_table_to);
    to.holder_ = from.holder_;
    to.holder_->count_.fetch_add(1, std::memory_order_relaxed);
  }
  static void destroy(cow& v, auto v_table) {
    if (v.holder_ && v_table != nullptr &&
        (v.holder_->count_.fetch_sub(1, std::memory_order_release) == 1)) {
      destruct(v_table, v.data_ptr());
      delete v.holder_;
    }
    v.holder_ = nullptr;
  }

  static void* get_proxy_ptr_in(cow const& v,
                                [[maybe_unused]] is_v_table auto* v_table) {
    return v.data_ptr();
  }
  static void* get_proxy_ptr_in(cow& v, is_v_table auto* v_table) {
    if (!v.unique()) {
      auto holder_size =
          sizeof(cow::holder<>) - sizeof(void*) + model_size(v_table).size;
      auto holder =
          new (static_cast<cow::holder_base*>(::operator new(holder_size)))
              cow::holder_base;
      copy_construct_at(v_table, cow::data_ptr_from_holder(holder),
                        cow::data_ptr_from_holder(v.holder_));
      destroy(v, v_table);
      v.holder_ = holder;
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

using namespace anyxx;

namespace {

template <typename T = std::string>
struct XX {
  static inline int tracker_ = 0;
  static inline int copy_constructed_ = 0;
  static inline int move_constructed_ = 0;
  static inline int copy_assigned_ = 0;
  static inline int move_assigned_ = 0;
  XX(T s = "") : s_(std::move(s)) {
    interior_ptr_ = &s_;
    std::println("X({})", s_);
    ++tracker_;
    std::println("X::tracker_ {})", tracker_);
  }
  ~XX() {  // NOLINT
    std::println("~X({} moved = {})", s_, moved_);
    if (!moved_) --tracker_;
    std::println("X::tracker_ {})", tracker_);
  }
  XX(XX const& x) : s_(x.s_) {
    interior_ptr_ = &s_;
    std::println("X(X const& {})", s_);
    ++tracker_;
    ++copy_constructed_;
    std::println("X::copy_constructed_ {})", copy_constructed_);
    std::println("X::tracker_ {})", tracker_);
  }
  XX(XX&& x) : s_(std::move(x.s_)) {  // NOLINT
    interior_ptr_ = &s_;
    std::println("X(X&& {})", s_);
    x.moved_ = true;
    ++move_constructed_;
    std::println("X::move_constructed_ {})", move_constructed_);
    std::println("X::tracker_ {})", tracker_);
  }
  XX& operator=(XX const& x) {
    s_ = x.s_;
    std::println("X =(X const& {})", s_);
    ++copy_assigned_;
    std::println("X::tracker_ {})", tracker_);
    return *this;
  }
  XX& operator=(XX&& x) {  // NOLINT
    std::println("X =(X&& {})", s_);
    x.moved_ = true;
    s_ = std::move(x.s_);
    ++move_assigned_;
    std::println("X::tracker_ {})", tracker_);
    return *this;
  }
  T const& get() const { return s_; }
  void set(T const& value) { s_ = value; }
  T* interior_ptr_ = nullptr;
  T s_;
  bool moved_ = false;
  [[nodiscard]] T operator()() const {
    CHECK(interior_ptr_ == &s_);
    return s_;
  }
};
using X = XX<>;

TRAIT_(getset, anyxx::dynamic_value,
       (ANY_FN(std::string const&, get, (), const),
        ANY_FN(void, set, (std::string const&), )))

using any_getset = any<getset, cow>;

}  // namespace

TEST_CASE("cow lifetime") {
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset u{std::in_place_type<X>, "hallo"};
      CHECK(u.get() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset u{std::in_place_type<X>, "hallo"};
      CHECK(u.get() == "hallo");
      CHECK(get_proxy(u).unique());
      CHECK(X::tracker_ == 1);
      any_getset u2{std::in_place_type<X>, "world"};
      CHECK(u2.get() == "world");
      CHECK(get_proxy(u2).unique());
      CHECK(X::tracker_ == 2);
      u2 = std::move(u);
      CHECK(get_proxy(u).holder_ == nullptr);  // NOLINT
      CHECK(u2.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u3 = std::move(u2);
      CHECK(get_proxy(u2).holder_ == nullptr);  // NOLINT
      CHECK(u3.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto u4 = move_to<any_getset>(std::move(u3));
      CHECK(get_proxy(u3).holder_ == nullptr);  // NOLINT
      CHECK(u4.get() == "hallo");
      CHECK(X::tracker_ == 1);
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset v{std::in_place_type<X>, "hallo"};
      CHECK(v.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = v;
      CHECK(X::tracker_ == 1);
      CHECK(get_proxy(v2).holder_ == get_proxy(v).holder_);
      CHECK(v.get() == "hallo");
      CHECK(v2.get() == "hallo");
      v2 = X{"world!"};
      CHECK(get_proxy(v).holder_->count_ == 1);
      CHECK(X::tracker_ == 2);
      CHECK(get_proxy(v2).holder_ != get_proxy(v).holder_);
      CHECK(v.get() == "hallo");
      CHECK(v2.get() == "world!");
    }
    CHECK(X::tracker_ == 0);
  }
  {
    CHECK(X::tracker_ == 0);
    {
      any_getset v{std::in_place_type<X>, "hallo"};
      CHECK(v.get() == "hallo");
      CHECK(X::tracker_ == 1);
      auto v2 = clone_to<any_getset>(v);
      CHECK(X::tracker_ == 1);
      CHECK(get_proxy(*v2).holder_ == get_proxy(v).holder_);
      CHECK(v.get() == "hallo");
      CHECK(v2->get() == "hallo");
      CHECK(get_proxy(*v2).holder_ == get_proxy(v).holder_);
      CHECK(get_proxy(v).holder_->count_ == 2);
      v2->set("hallo world");
      CHECK(v.get() == "hallo");
      CHECK(v2->get() == "hallo world");
      CHECK(get_proxy(*v2).holder_ != get_proxy(v).holder_);
      CHECK(get_proxy(*v2).unique());
      CHECK(get_proxy(v).unique());
    }
    CHECK(X::tracker_ == 0);
    X::tracker_ = 0;
  }
}
