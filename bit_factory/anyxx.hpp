#pragma once

//
// for Microsoft C++, you must enable the C-Preprocessor with this flag:
// /Zc:preprocessor (see CMakeLists.txt for example)
//

#include <cassert>
#include <concepts>
#include <cstddef>
#include <expected>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#ifdef __clang__
#pragma GCC diagnostic ignored "-Wcast-function-type-mismatch"
#pragma GCC diagnostic ignored "-Wmicrosoft-template-shadow"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wextra-semi"
#endif

namespace anyxx {

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class type_mismatch_error : public error {
  using error::error;
};

using const_void = void const*;
using mutable_void = void*;
template <typename V>
concept voidness =
    (std::same_as<V, const_void> || std::same_as<V, mutable_void>);
template <voidness Voidness>
struct is_const_void_;
template <>
struct is_const_void_<void*> : std::false_type {};
template <>
struct is_const_void_<void const*> : std::true_type {};
template <typename Voidness>
concept is_const_void = is_const_void_<Voidness>::value;

class meta_data;

template <typename U>
bool type_match(meta_data const& meta);

template <typename U>
void check_type_match(meta_data const& meta) {
  if (!type_match<U>(meta)) throw type_mismatch_error("type mismatch");
}

template <typename Data>
struct trait;

template <class E>
concept is_erased_data = requires(E e) {
  typename trait<E>::void_t;
  { trait<E>::is_constructibile_from_const } -> std::convertible_to<bool>;
  { trait<E>::is_owner } -> std::convertible_to<bool>;
  { trait<E>::value(e) } -> std::convertible_to<typename trait<E>::void_t>;
  { trait<E>::has_value(e) } -> std::convertible_to<bool>;
  { trait<E>::is_weak } -> std::convertible_to<bool>;
  { trait<E>::default_construct() };
};

template <is_erased_data Data>
using data_void = trait<Data>::void_t;

template <typename ErasedData>
concept is_const_data =
    is_erased_data<ErasedData> && is_const_void<data_void<ErasedData>>;

template <typename ErasedData>
concept is_weak_data = is_erased_data<ErasedData> && trait<ErasedData>::is_weak;

template <bool CallIsConst, bool ErasedDataIsConst, bool ErasedDataIsWeak>
concept const_correct_call =
    !ErasedDataIsWeak &&
    ((CallIsConst == ErasedDataIsConst) || !ErasedDataIsConst);

template <typename CALL, typename ErasedData, bool Exact>
concept const_correct_call_for_erased_data =
    !is_weak_data<ErasedData> && voidness<CALL> && is_erased_data<ErasedData> &&
    ((Exact && (is_const_void<CALL> == is_const_data<ErasedData>)) ||
     (!Exact &&
      const_correct_call<is_const_void<CALL>, is_const_data<ErasedData>,
                         is_weak_data<ErasedData>>));

template <is_erased_data ErasedData, typename From>
ErasedData erased(From&& from) {
  return trait<ErasedData>::erase(std::forward<From>(from));
}

template <is_erased_data ErasedData, typename ConstructedWith>
using unerased =
    trait<ErasedData>::template unerased<std::decay_t<ConstructedWith>>;

template <is_erased_data ErasedData>
bool has_data(ErasedData const& vv) {
  return trait<ErasedData>::has_value(vv);
}
template <is_erased_data ErasedData>
void const* get_void_data_ptr(ErasedData const& vv)
  requires std::same_as<void const*, typename trait<ErasedData>::void_t>
{
  return trait<ErasedData>::value(vv);
}
template <is_erased_data ErasedData>
void* get_void_data_ptr(ErasedData const& vv)
  requires std::same_as<void*, typename trait<ErasedData>::void_t>
{
  return trait<ErasedData>::value(vv);
}

template <typename U>
auto unchecked_unerase_cast(void const* p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(void* p) {
  return static_cast<U*>(p);
}

template <typename U, is_erased_data ErasedData>
auto unchecked_unerase_cast(ErasedData const& o) {
  return unchecked_unerase_cast<U>(get_void_data_ptr(o));
}
template <typename U, is_erased_data ErasedData>
auto unchecked_unerase_cast(ErasedData const& o)
  requires(!is_const_data<ErasedData>)
{
  return unchecked_unerase_cast<U>(get_void_data_ptr(o));
}

template <typename U, is_erased_data ErasedData>
auto unerase_cast(ErasedData const& o, meta_data const& meta) {
  check_type_match<U>(meta);
  return unchecked_unerase_cast<U>(o);
}
template <typename U, is_erased_data ErasedData>
U const* unerase_cast_if(ErasedData const& o, meta_data const& meta) {
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(o);
  return nullptr;
}
template <typename U, is_erased_data ErasedData>
U* unerase_cast_if(ErasedData const& o, meta_data const& meta)
  requires(!is_const_data<ErasedData>)
{
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(o);
  return nullptr;
}

template <typename ConstructedWith, typename ErasedData, typename BASE>
concept erased_constructibile_for =
    !std::derived_from<std::remove_cvref_t<ConstructedWith>, BASE> &&
    !is_erased_data<std::remove_cvref_t<ConstructedWith>> &&
    (!std::is_const_v<std::remove_reference_t<ConstructedWith>> ||
     trait<ErasedData>::is_constructibile_from_const);

// --------------------------------------------------------------------------------
// erased data observer

template <voidness Voidness>
using observer = Voidness;
using const_observer = observer<const_void>;
using mutable_observer = observer<mutable_void>;

template <typename ErasedData, voidness Voidness>
struct observer_trait {
  using void_t = Voidness;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;
  static constexpr bool is_owner = false;
  static constexpr bool is_weak = false;
  static auto default_construct() { return void_t{}; }

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  static Voidness value(const auto& ptr) { return ptr; }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

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
  static auto erase(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return ErasedData(static_cast<Voidness>(&v));
  }
  template <typename V>
  static auto erase(const V& v)
    requires(is_const)
  {
    return ErasedData(static_cast<Voidness>(&v));
  }
};

template <>
struct trait<const_observer> : observer_trait<const_observer, const_observer> {
};
template <>
struct trait<mutable_observer>
    : observer_trait<mutable_observer, mutable_observer> {};

static_assert(trait<const_observer>::is_const);
static_assert(!trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<const_observer>);

// --------------------------------------------------------------------------------
// erased data shared_const + weak

using shared_const = std::shared_ptr<void const>;
using weak = std::weak_ptr<void const>;

template <>
struct trait<shared_const> {
  using void_t = void const*;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_owner = true;
  static constexpr bool is_weak = false;
  static auto default_construct() { return shared_const{}; }

  static void const* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

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
};

template <>
struct trait<weak> {
  using void_t = void const*;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_owner = false;
  static constexpr bool is_weak = true;
  static auto default_construct() { return weak{}; }

  static void const* value([[maybe_unused]] const auto& ptr) { return nullptr; }
  static bool has_value(const auto& ptr) { return !ptr.expired(); }

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
  static auto erase(std::weak_ptr<V> const& v) {
    return static_pointer_cast<void const>(v);
  }
};

static_assert(is_erased_data<shared_const>);
static_assert(is_erased_data<weak>);

// --------------------------------------------------------------------------------
// erased data unique
template <typename T>
auto erased_delete() {
  return +[](void* data) { delete static_cast<T*>(data); };
}

using void_deleter_t = void (*)(void*);
using unique = std::unique_ptr<void, void_deleter_t>;

template <typename T>
auto move_to_unique(std::unique_ptr<T> p) {
  return unique(p.release(), erased_delete<T>());
}

template <typename T, typename... Args>
auto make_unique([[maybe_unused]] Args&&... args) {
  return move_to_unique(std::make_unique<T>(std::forward<Args>(args)...));
}

inline unique unique_nullptr() {
  return {nullptr, [](auto) {}};
}

template <>
struct trait<unique> {
  using void_t = void*;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_owner = true;
  static constexpr bool is_weak = false;
  static auto default_construct() { return unique_nullptr(); }

  static void* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

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
    return make_unique<V>(std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return make_unique<T>(std::forward<Args>(args)...);
  }
  template <typename V>
  static auto erase(std::unique_ptr<V>&& v) {
    return move_to_unique(std::move(v));
  }
};

static_assert(is_erased_data<unique>);

// --------------------------------------------------------------------------------
// erased data value

struct value_v_table {
  using destroy_fn = void(void*) noexcept;
  using copy_fn = void*(const void*);
  template <class Data>
  constexpr value_v_table(std::in_place_type_t<Data>)
      : destroy(&destroy_impl<Data>), copy(&copy_impl<Data>) {}
  template <class Data>
  static void destroy_impl(void* target) noexcept {
    ::delete static_cast<Data*>(target);
  }
  template <class Data>
  static void* copy_impl(void const* source) {
    return ::new Data(*static_cast<Data const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class Data>
constexpr value_v_table value_v_table_of =
    value_v_table(std::in_place_type<Data>);

class value {
 public:
  value() = default;
  template <typename Data>
  value(Data* ptr)
      : ptr_(ptr), v_table_(&value_v_table_of<std::decay_t<Data>>) {}
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

template <typename T, typename... Args>
auto make_value(Args&&... args) {
  return value(new T(std::forward<Args>(args)...));
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
  static auto default_construct() { return anyxx::value{}; }

  static void* value(const auto& v) { return v.get(); }
  static bool has_value(const auto& v) { return v; }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return make_value<V>(std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return make_value<T>(std::forward<Args>(args)...);
  }
  template <typename V>
  static auto erase(std::unique_ptr<V>&& v) {
    return make_value<std::decay_t<V>>(std::move(v));
  }
  template <typename ConstructedWith>
  static auto erase(ConstructedWith&& v) {
    return make_value<std::decay_t<ConstructedWith>>(
        std::forward<ConstructedWith>(v));
  }
};

static_assert(is_erased_data<value>);

// --------------------------------------------------------------------------------
// meta data

class meta_data;
struct any_base_v_table;

template <typename TYPE>
auto& runtime_implementation();

#ifdef ANY_DLL_MODE
template <typename T>
const std::type_info& typeid_of();
template <typename T>
meta_data& get_meta_data();
#else
template <typename T>
const std::type_info& typeid_of() {
  return typeid(T);
}
template <typename T>
meta_data& get_meta_data() {
  return runtime_implementation<std::decay_t<T>>();
}
#endif

struct any_base_v_table {
  template <typename Concrete>
  any_base_v_table([[maybe_unused]] std::in_place_type_t<Concrete> concrete);

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(any_base_v_table) == from;
  }

  meta_data* meta_data_ = nullptr;

  bool (*_is_derived_from)(const std::type_info&);
};

inline bool is_derived_from(const std::type_info& from,
                            any_base_v_table const* any_base_v_table) {
  return any_base_v_table->_is_derived_from(from);
}

using dispatch_table_function_t = void (*)();
using dispatch_table_dispatch_index_t = std::size_t;
using dispatch_table_entry_t = unsigned long long;
using dispatch_table_t = std::vector<dispatch_table_entry_t>;

void insert_function(dispatch_table_t* table, std::size_t index, auto fp) {
  if (table->size() <= index) table->resize(index + 1);
  auto& entry = table->at(index);
  entry = reinterpret_cast<unsigned long long>(fp);
}
inline dispatch_table_function_t get_function(dispatch_table_t* table,
                                              std::size_t index) {
  if (table->size() <= index) return {};
  return reinterpret_cast<dispatch_table_function_t>(table->at(index));
}

inline dispatch_table_dispatch_index_t get_multi_dispatch_index_at(
    dispatch_table_t* table, std::size_t index) {
  if (table->size() <= index) return {};
  if (auto const entry = table->at(index))
    return static_cast<dispatch_table_dispatch_index_t>(entry);
  else
    return {};
}
inline void set_multi_dispatch_index_at(
    dispatch_table_t* table, std::size_t index_multi_dispatch,
    dispatch_table_dispatch_index_t
        dispatch_index_of_class_in_dispatch_matrix) {
  if (table->size() <= index_multi_dispatch)
    table->resize(index_multi_dispatch + 1);
  auto& entry = table->at(index_multi_dispatch);
  entry = dispatch_index_of_class_in_dispatch_matrix;
}

struct cast_error {
  std::type_info const &to, &from;
};

class meta_data {
  const std::type_info& type_info_;
  using copy_construct_t = auto(const_void) -> unique;
  copy_construct_t* copy_construct_;

  std::vector<any_base_v_table*> i_table_;

 public:
  template <typename CLASS>
  constexpr meta_data(std::in_place_type_t<CLASS>)
      : type_info_(typeid_of<CLASS>()),
        copy_construct_(+[]([[maybe_unused]] const_void from) {
          if constexpr (std::is_copy_constructible_v<CLASS>) {
            return erased<unique>(
                std::make_unique<CLASS>(*static_cast<CLASS const*>(from)));
          } else {
            return unique_nullptr();
          }
        }) {}

  constexpr const std::type_info& get_type_info() const { return type_info_; }
  auto copy_construct(const_void from) const { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  std::expected<any_base_v_table*, cast_error> get_v_table(
      std::type_info const& typeid_) const {
    auto const& i_table = get_i_table();
    for (auto v_table : i_table)
      if (is_derived_from(typeid_, v_table)) return v_table;
    return std::unexpected(cast_error{.to = typeid_, .from = get_type_info()});
  }
  auto register_v_table(any_base_v_table* v_table) {
    v_table->meta_data_ = this;
    if (std::ranges::find(get_i_table(), v_table) == get_i_table().end())
      i_table_.push_back(v_table);
    return v_table;
  }
};

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data_{std::in_place_type<TYPE>};
  return meta_data_;
}

template <typename Concrete>
any_base_v_table::any_base_v_table(
    [[maybe_unused]] std::in_place_type_t<Concrete> concrete)
    : _is_derived_from([](const std::type_info& from) {
        return static_is_derived_from(from);
      }) {}

template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance_implementation() {
  static dispatch_table_t dispatch_table;
  return &dispatch_table;
}

#ifdef ANY_DLL_MODE

template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance();

#else

template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance() {
  return dispatch_table_instance_implementation<AnyVTable, Class>();
}

#endif  //

template <typename VTable, typename Concrete>
auto bind_v_table_to_meta_data() {
  auto v_table = VTable::template imlpementation<Concrete>();
  get_meta_data<Concrete>().register_v_table(v_table);
  return v_table;
}

template <typename U>
bool type_match(meta_data const& meta) {
  return &meta.get_type_info() == &typeid_of<std::decay_t<U>>();
}

// --------------------------------------------------------------------------------
// borrow erased data

template <is_erased_data To, is_erased_data From>
struct borrow_trait;

template <typename To, typename From>
concept borrowable_from =
    is_erased_data<From> && is_erased_data<To> && requires(From f) {
      { borrow_trait<To, From>{}(f) } -> std::same_as<To>;
    };

template <is_erased_data From>
  requires(!is_const_data<From> && !is_weak_data<From>)
struct borrow_trait<mutable_observer, From> {
  auto operator()(const auto& from) const {
    return mutable_observer{get_void_data_ptr(from)};
  }
};
template <is_erased_data From>
  requires(!is_weak_data<From>)
struct borrow_trait<const_observer, From> {
  auto operator()(const auto& from) const {
    return const_observer{get_void_data_ptr(from)};
  }
};
template <>
struct borrow_trait<shared_const, shared_const> {
  auto operator()(const auto& from) const { return from; }
};
template <>
struct borrow_trait<weak, weak> {
  auto operator()(const auto& from) const { return from; }
};
template <>
struct borrow_trait<weak, shared_const> {
  auto operator()(const auto& from) const { return weak{from}; }
};

template <typename To, typename From>
  requires borrowable_from<To, From>
To borrow_as(From const& from) {
  return borrow_trait<To, From>{}(from);
}

static_assert(!borrowable_from<mutable_observer, const_observer>);
static_assert(borrowable_from<mutable_observer, mutable_observer>);
static_assert(borrowable_from<mutable_observer, unique>);
static_assert(!borrowable_from<mutable_observer, shared_const>);
static_assert(!borrowable_from<mutable_observer, weak>);
static_assert(borrowable_from<mutable_observer, value>);

static_assert(borrowable_from<const_observer, const_observer>);
static_assert(borrowable_from<const_observer, mutable_observer>);
static_assert(borrowable_from<const_observer, unique>);
static_assert(borrowable_from<const_observer, shared_const>);
static_assert(!borrowable_from<const_observer, weak>);
static_assert(borrowable_from<const_observer, value>);

static_assert(!borrowable_from<shared_const, const_observer>);
static_assert(!borrowable_from<shared_const, mutable_observer>);
static_assert(!borrowable_from<shared_const, unique>);
static_assert(borrowable_from<shared_const, shared_const>);
static_assert(!borrowable_from<shared_const, weak>);
static_assert(!borrowable_from<shared_const, value>);

static_assert(!borrowable_from<weak, const_observer>);
static_assert(!borrowable_from<weak, mutable_observer>);
static_assert(!borrowable_from<weak, unique>);
static_assert(borrowable_from<weak, shared_const>);
static_assert(borrowable_from<weak, weak>);
static_assert(!borrowable_from<weak, value>);

static_assert(!borrowable_from<unique, const_observer>);
static_assert(!borrowable_from<unique, mutable_observer>);
static_assert(!borrowable_from<unique, unique>);
static_assert(!borrowable_from<unique, shared_const>);
static_assert(!borrowable_from<unique, weak>);
static_assert(!borrowable_from<unique, value>);

static_assert(!borrowable_from<value, const_observer>);
static_assert(!borrowable_from<value, mutable_observer>);
static_assert(!borrowable_from<value, unique>);
static_assert(!borrowable_from<value, shared_const>);
static_assert(!borrowable_from<value, weak>);
static_assert(!borrowable_from<value, value>);

// --------------------------------------------------------------------------------
// clone erased data

template <is_erased_data TOFROM>
struct can_copy_to;

template <typename To>
concept cloneable_to = is_erased_data<To> && trait<To>::is_owner;

template <is_erased_data To, is_erased_data From>
  requires cloneable_to<To>
To clone_to(From const& from, auto const& meta_data) {
  return meta_data.copy_construct(get_void_data_ptr(from));
}

static_assert(!cloneable_to<mutable_observer>);
static_assert(!cloneable_to<const_observer>);
static_assert(cloneable_to<shared_const>);
static_assert(!cloneable_to<weak>);
static_assert(cloneable_to<unique>);
static_assert(cloneable_to<value>);

// --------------------------------------------------------------------------------
// move erased data

template <typename To, typename From>
inline static bool constexpr can_move_to_from = false;

template <typename To, typename From>
concept moveable_from =
    is_erased_data<From> && is_erased_data<To> && can_move_to_from<To, From>;

template <is_erased_data X>
inline static bool constexpr can_move_to_from<X, X> = true;

template <>
inline bool constexpr can_move_to_from<shared_const, unique> = true;

template <>
inline bool constexpr can_move_to_from<weak, shared_const> = true;

template <voidness To, voidness From>
  requires const_correct_call<is_const_void<To>, is_const_void<From>,
                              is_weak_data<From>>
inline static bool constexpr can_move_to_from<To, From> = true;

template <typename To, typename From>
  requires moveable_from<To, std::decay_t<From>>
To move_to(From&& from) {
  return std::move(from);
}

static_assert(!moveable_from<mutable_observer, const_observer>);
static_assert(moveable_from<mutable_observer, mutable_observer>);
static_assert(!moveable_from<mutable_observer, unique>);
static_assert(!moveable_from<mutable_observer, shared_const>);
static_assert(!moveable_from<mutable_observer, weak>);
static_assert(!moveable_from<mutable_observer, value>);

static_assert(moveable_from<const_observer, const_observer>);
static_assert(moveable_from<const_observer, mutable_observer>);
static_assert(!moveable_from<const_observer, unique>);
static_assert(!moveable_from<const_observer, shared_const>);
static_assert(!moveable_from<const_observer, weak>);
static_assert(!moveable_from<const_observer, value>);

static_assert(!moveable_from<shared_const, const_observer>);
static_assert(!moveable_from<shared_const, mutable_observer>);
static_assert(moveable_from<shared_const, unique>);
static_assert(moveable_from<shared_const, shared_const>);
static_assert(!moveable_from<shared_const, weak>);
static_assert(!moveable_from<shared_const, value>);

static_assert(!moveable_from<weak, const_observer>);
static_assert(!moveable_from<weak, mutable_observer>);
static_assert(!moveable_from<weak, unique>);
static_assert(moveable_from<weak, shared_const>);
static_assert(moveable_from<weak, weak>);
static_assert(!moveable_from<weak, value>);

static_assert(!moveable_from<unique, const_observer>);
static_assert(!moveable_from<unique, mutable_observer>);
static_assert(moveable_from<unique, unique>);
static_assert(!moveable_from<unique, shared_const>);
static_assert(!moveable_from<unique, weak>);
static_assert(!moveable_from<unique, value>);

static_assert(!moveable_from<value, const_observer>);
static_assert(!moveable_from<value, mutable_observer>);
static_assert(!moveable_from<value, unique>);
static_assert(!moveable_from<value, shared_const>);
static_assert(!moveable_from<value, weak>);
static_assert(moveable_from<value, value>);

// --------------------------------------------------------------------------------
// any base

template <is_erased_data ErasedData>
class any_base;

template <typename I>
concept is_erased_data_holder_impl = requires(I i) {
  typename I::void_t;
  typename I::erased_data_t;
  typename I::trait_t;
  { get_erased_data(i) };
};
template <typename I>
concept is_erased_data_holder = is_erased_data_holder_impl<std::decay_t<I>>;

template <typename I>
concept is_any_impl =
    is_erased_data_holder_impl<I> &&
    requires(I i, I::erased_data_t ed) { typename I::v_table_t; };
template <typename I>
concept is_any = is_any_impl<std::decay_t<I>>;

template <class E>
concept is_typed_any = is_any<E> && requires(E e) {
  typename E::trait_t;
  typename E::value_t;
  { E::is_const } -> std::convertible_to<bool>;
};

template <typename ConstructedWith, typename ErasedData>
concept constructibile_for =
    erased_constructibile_for<ConstructedWith, ErasedData,
                              any_base<ErasedData>> &&
    !is_erased_data_holder<ConstructedWith> &&
    !is_typed_any<std::remove_cvref_t<ConstructedWith>>;

template <is_erased_data ErasedData>
class erased_data_holder {
 public:
  using erased_data_t = ErasedData;
  using trait_t = trait<erased_data_t>;
  using void_t = typename trait_t::void_t;

 protected:
  erased_data_t erased_data_ = trait_t::default_construct();

  erased_data_holder() = default;
  erased_data_holder(erased_data_t erased_data)
      : erased_data_(std::move(erased_data)) {}
  template <typename ConstructedWith>
  erased_data_holder(ConstructedWith&& constructed_with)
    requires constructibile_for<ConstructedWith, ErasedData>
      : erased_data_(erased<erased_data_t>(
            std::forward<ConstructedWith>(constructed_with))) {}
  template <typename V>
  erased_data_holder(std::in_place_t, V&& v)
      : erased_data_(
            trait<ErasedData>::construct_in_place(std::forward<V>(v))) {}
  template <typename T, typename... Args>
  erased_data_holder(std::in_place_type_t<T>, Args&&... args)
      : erased_data_(trait<ErasedData>::template construct_type_in_place<T>(
            std::forward<Args>(args)...)) {}

 public:
  template <is_erased_data_holder Other>
  erased_data_holder(const Other& other)
    requires(borrowable_from<erased_data_t, typename Other::erased_data_t>)
      : erased_data_(borrow_as<ErasedData>(other.erased_data_)) {}
  template <typename Other>
  erased_data_holder(Other&& other)
    requires(moveable_from<erased_data_t, typename Other::erased_data_t>)
      : erased_data_(move_to<ErasedData>(std::move(other.erased_data_))) {}
  template <typename Other>
  erased_data_holder& operator=(Other&& other) {
    erased_data_ = move_to<ErasedData>(std::move(other.erased_data_));
    return *this;
  }
  erased_data_holder(const erased_data_holder&) = default;
  // any_base(any_base&) requires(std::is_copy_constructible_v<any_base>) =
  // default;
  erased_data_holder(erased_data_holder&& rhs) noexcept
      : erased_data_(std::move(rhs.erased_data_)) {}
  erased_data_holder& operator=(erased_data_holder const& other) = default;
  erased_data_holder& operator=(erased_data_holder&& other) = default;

  template <is_erased_data FriendsErasedData>
  friend inline auto& get_erased_data(
      erased_data_holder<FriendsErasedData> const& any);
  template <is_erased_data FriendsErasedData>
  friend inline auto move_erased_data(
      erased_data_holder<FriendsErasedData>&& any);
  template <is_erased_data FriendsErasedData>
  friend inline auto get_void_data_ptr(
      erased_data_holder<FriendsErasedData> const& any);
};

template <is_erased_data ErasedData>
class any_base : public erased_data_holder<ErasedData> {
 public:
  using erased_data_holder_t = erased_data_holder<ErasedData>;
  using erased_data_t = typename erased_data_holder_t::erased_data_t;
  using trait_t = typename erased_data_holder_t::trait_t;
  using void_t = typename erased_data_holder_t::void_t;
  using v_table_t = any_base_v_table;

 protected:
  v_table_t* v_table_ = nullptr;

  any_base() = default;
  any_base(erased_data_t erased_data, v_table_t* v_table)
      : erased_data_holder_t(std::move(erased_data)), v_table_(v_table) {}
  template <typename ConstructedWith>
  any_base(ConstructedWith&& constructed_with)
    requires constructibile_for<ConstructedWith, ErasedData>
      : erased_data_holder_t(std::forward<ConstructedWith>(constructed_with)) {}
  template <typename V>
  any_base(std::in_place_t in_place, V&& v)
      : erased_data_holder_t(in_place, std::forward<V>(v)) {}
  template <typename T, typename... Args>
  any_base(std::in_place_type_t<T> in_place_type, Args&&... args)
      : erased_data_holder_t(in_place_type, std::forward<Args>(args)...) {}

 public:
  template <is_any Other>
  any_base(const Other& other)
    requires(std::derived_from<typename Other::v_table_t, v_table_t> &&
             borrowable_from<erased_data_t, typename Other::erased_data_t>)
      : erased_data_holder_t(other), v_table_(get_v_table(other)) {}
  template <typename Other>
  any_base(Other&& other)
    requires(std::derived_from<typename Other::v_table_t, v_table_t> &&
             moveable_from<erased_data_t, typename Other::erased_data_t>)
      : erased_data_holder_t(std::move(other.erased_data_)),
        v_table_(get_v_table(other)) {}
  template <typename Other>
  any_base& operator=(Other&& other)
    requires(std::derived_from<typename Other::v_table_t, v_table_t>)
  {
    static_cast<erased_data_holder_t&>(*this) = std::move(other.erased_data_);
    v_table_ = get_v_table(other);
    return *this;
  }
  any_base(const any_base&) = default;
  // any_base(any_base&) requires(std::is_copy_constructible_v<any_base>) =
  // default;
  any_base(any_base&& rhs) noexcept
      : erased_data_holder_t(std::move(rhs.erased_data_)),
        v_table_(rhs.v_table_) {}
  any_base& operator=(any_base const& other) = default;
  any_base& operator=(any_base&& other) = default;

  template <is_erased_data Other>
  friend class any_base;

  template <is_any I>
  friend inline auto get_v_table(I const& any);

  template <is_any To, is_any From>
  friend inline To unchecked_downcast_to(From from)
    requires(std::derived_from<To, From>);
};

template <is_erased_data ErasedData>
inline auto& get_erased_data(erased_data_holder<ErasedData> const& any) {
  return any.erased_data_;
}
template <is_erased_data ErasedData>
inline auto move_erased_data(erased_data_holder<ErasedData>&& any) {
  return std::move(any.erased_data_);
}
template <is_erased_data ErasedData>
inline auto get_void_data_ptr(erased_data_holder<ErasedData> const& any) {
  return get_void_data_ptr(get_erased_data(any));
}

template <is_any Any>
inline const auto& get_meta_data(Any const& any) {
  return *get_v_table(any)->meta_data_;
}

template <is_erased_data VV>
bool is_derived_from(const std::type_info& from, any_base<VV> const& any) {
  return get_v_table(any)->_is_derived_from(from);
}
template <is_any From, is_erased_data VV>
bool is_derived_from(any_base<VV> const& any) {
  return is_derived_from(typeid(typename From::v_table_t), any);
}

template <typename VTable>
void set_is_derived_from(auto v_table) {
  v_table->_is_derived_from = +[](const std::type_info& from) {
    return VTable::static_is_derived_from(from);
  };
}

template <typename To>
auto unchecked_v_table_downcast_to(any_base_v_table* v_table) {
  return static_cast<To*>(v_table);
}
template <is_any To>
auto unchecked_v_table_downcast_to(any_base_v_table* v_table) {
  return unchecked_v_table_downcast_to<typename To::v_table_t>(v_table);
}

template <is_any Any>
inline auto get_v_table(Any const& any) {
  return unchecked_v_table_downcast_to<Any>(any.v_table_);
}

template <is_any To, is_any From>
inline To unchecked_downcast_to(From from)
  requires(std::derived_from<To, From>)
{
  return To{std::move(from.erased_data_),
            unchecked_v_table_downcast_to<To>(from.v_table_)};
}

template <is_any To, is_any From>
inline std::optional<To> downcast_to(From from)
  requires(std::derived_from<To, From>)
{
  if (is_derived_from<To>(from))
    return {unchecked_downcast_to<To>(std::move(from))};
  return {};
}

template <typename U, is_any Any>
inline auto unchecked_unerase_cast(Any const& o) {
  return unchecked_unerase_cast<U>(get_erased_data(o));
}
template <typename U, is_any Any>
inline auto unerase_cast(Any const& o) {
  return unerase_cast<U>(get_erased_data(o), get_meta_data(o));
}
template <typename U, is_any Any>
inline auto unerase_cast_if(Any const& o) {
  return unerase_cast_if<U>(get_erased_data(o), get_meta_data(o));
}

#ifdef ANY_DLL_MODE
template <typename VTable, typename Concrete>
VTable* v_table_instance_implementaion();
#else
template <typename VTable, typename Concrete>
VTable* v_table_instance_implementaion() {
  static VTable v_table{std::in_place_type<Concrete>};
  [[maybe_unused]] static auto __ =
      anyxx::get_meta_data<Concrete>().register_v_table(&v_table);
  return &v_table;
}
#endif  // DEBUG

template <template <typename...> typename Any>
constexpr bool has_dispatchs = false;

template <typename I>
concept has_dispatchs_enabled = is_any<I> && I::v_table_t::dispatchs_enabled;

template <bool HasDispatch, template <typename...> typename Any>
struct dispatch_holder;
template <template <typename...> typename Any>
struct dispatch_holder<false, Any> {
  static void set_dispatch_table([[maybe_unused]] dispatch_table_t* t) {}
};
template <template <typename...> typename Any>
struct dispatch_holder<true, Any> {
  void set_dispatch_table(dispatch_table_t* t) { dispatch_table = t; }
  dispatch_table_t* dispatch_table = nullptr;
};

template <is_any ToAny>
auto query_v_table(const meta_data& meta_data)
    -> std::expected<typename ToAny::v_table_t*, anyxx::cast_error> {
  using v_table_t = typename ToAny::v_table_t;
  return meta_data.get_v_table(typeid(v_table_t)).transform([](auto v_table) {
    return static_cast<v_table_t*>(v_table);
  });
}

template <typename ToAny>
auto query_v_table(any_base_v_table* from) {
  return find_v_table<ToAny>(*from->meta_data_);
}

// --------------------------------------------------------------------------------
// typed any

template <typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
struct typed_any : public Any<ErasedData> {
  using erased_data_t = ErasedData;
  using any_t = Any<ErasedData>;
  using trait_t = any_t::trait_t;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using any_t::any_t;

  typed_any(V const& v) : any_t(v) {}
  typed_any(V&& v) : any_t(std::move(v)) {}
  typed_any(any_t i) : any_t(i) { check_type_match<V>(get_meta_data(*this)); }

  value_t const& operator*() const {
    return *unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t const* operator->() const {
    return unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t const* get() const {
    return unchecked_unerase_cast<value_t const>(this->erased_data_);
  }
  value_t& operator*() const
    requires(!is_const)
  {
    return *unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* operator->() const
    requires(!is_const)
  {
    return unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* get() const
    requires(!is_const)
  {
    return unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  explicit operator bool() const {
    return static_cast<bool>(this->erased_data_);
  }
};

template <typename V, template <is_erased_data> typename Any>
struct bound_typed_any_impl {
  template <is_erased_data ErasedData>
  using type = typed_any<V, Any, ErasedData>;
};
template <typename V, template <is_erased_data> typename Any>
using bound_typed_any = bound_typed_any_impl<V, Any>;

template <typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
bool has_data(typed_any<V, Any, ErasedData> const& vv) {
  return has_data(vv.erased_data_);
}
template <typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
void const* get_void_data_ptr(typed_any<V, Any, ErasedData> const& vv)
  requires is_const_void<typename Any<ErasedData>::void_t>
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
void* get_void_data_ptr(typed_any<V, Any, ErasedData> const& vv)
  requires(!is_const_void<typename Any<ErasedData>::void_t>)
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
auto get_meta(typed_any<V, Any, ErasedData> const& vv) {
  using trait_t = typename Any<ErasedData>::trait_t;
  return trait_t::meta(vv.erased_data_);
}

template <typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
auto as(Any<ErasedData> source) {
  return typed_any<V, Any, ErasedData>{std::move(source)};
}

template <typename To, typename V, template <is_erased_data> typename Any,
          is_erased_data ErasedData>
auto as(typed_any<V, Any, ErasedData> source)
  requires std::convertible_to<V*, To*>
{
  if constexpr (typed_any<V, Any, ErasedData>::is_const) {
    return typed_any<To const, Any, ErasedData>{std::move(source.erased_data_)};
  } else {
    return typed_any<To, Any, ErasedData>{std::move(source.erased_data_)};
  }
}

// --------------------------------------------------------------------------------
// any borrow, clone, lock, move

template <is_any ToAny, is_erased_data FromErasedData>
  requires borrowable_from<typename ToAny::erased_data_t, FromErasedData>
std::expected<ToAny, cast_error> borrow_as(FromErasedData const& from,
                                           const meta_data& meta_data) {
  using to = typename ToAny::erased_data_t;
  return query_v_table<ToAny>(meta_data).transform(
      [&](auto v_table) { return ToAny{borrow_as<to>(from), v_table}; });
}

template <is_any ToAny, is_any FromAny>
  requires borrowable_from<typename ToAny::erased_data_t,
                           typename FromAny::erased_data_t>
std::expected<ToAny, cast_error> borrow_as(FromAny const& from) {
  if constexpr (std::same_as<typename ToAny::v_table_t,
                             typename FromAny::v_table_t>) {
    return from;
  } else if constexpr (std::derived_from<typename ToAny::v_table_t,
                                         typename FromAny::v_table_t>) {
    if (auto to = downcast_to<ToAny>(from)) return *to;
  }
  return borrow_as<ToAny>(get_erased_data(from), get_meta_data(from));
}

template <is_any ToAny, is_any FromAny>
std::expected<ToAny, cast_error> clone_to(FromAny const& from) {
  using vv_to_t = typename ToAny::erased_data_t;
  auto& meta_data = get_meta_data(from);
  static_assert(is_erased_data<vv_to_t>);
  return query_v_table<ToAny>(meta_data).transform([&](auto v_table) {
    return ToAny{clone_to<vv_to_t>(get_erased_data(from), meta_data), v_table};
  });
}

template <is_any FromAny>
  requires std::same_as<typename FromAny::erased_data_t, weak>
auto lock(FromAny const& from_interface) {
  using to_interface_t = FromAny::template type_for<shared_const>;
  static_assert(is_any<to_interface_t>);
  using return_t = std::optional<to_interface_t>;
  if (auto locked = get_erased_data(from_interface).lock())
    return return_t{{locked, get_v_table(from_interface)}};
  return return_t{};
}

template <is_any ToAny, is_erased_data FromAny>
ToAny move_to(FromAny&& vv_from, const meta_data& get_meta_data) {
  using vv_to_t = typename ToAny::erased_data_t;
  static_assert(is_erased_data<vv_to_t>);
  auto v_table = query_v_table<ToAny>(get_meta_data);
  return ToAny{move_to<vv_to_t>(std::move(vv_from)), *v_table};
}

template <is_any ToAny, is_any FromAny>
ToAny move_to(FromAny&& from_interface) {
  return move_to<ToAny>(move_erased_data(std::move(from_interface)),
                        get_meta_data(from_interface));
}

// --------------------------------------------------------------------------------
// hook

template <typename R, typename... Args>
class hook;
template <typename R, typename... Args>
class hook<R(Args...)> {
 public:
  class connection {
    int id_;
    hook* hook_;
    friend class hook;

    connection(int id, hook* hook) : id_(id), hook_(hook) {}
    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

   public:
    connection(connection&&) = default;
    connection& operator=(connection&&) = default;
    void close() {
      if (hook_) hook_->remove(id_);
      hook_ = nullptr;
    }

    ~connection() { close(); }
  };

  class super {
    int index_;
    hook const& hook_;
    friend class hook;

    super(int index, hook const& hook) : index_(index), hook_(hook) {}

   public:
    explicit operator bool() const { return index_ >= 0; }
    R operator()(Args&&... args) const {
      assert(index_ >= 0);
      return hook_.callees_[index_].second(super{index_ - 1, hook_},
                                           std::forward<Args>(args)...);
    }
  };

  using callee = std::function<R(super const&, Args...)>;

  R operator()(Args&&... args) const {
    assert(!callees_.empty());
    return callees_.back().second(super{((int)callees_.size()) - 2, *this},
                                  std::forward<Args>(args)...);
  }

  [[nodiscard]]
  connection insert(callee const& f) {
    callees_.emplace_back(entry{next_id_, f});
    return connection{next_id_++, this};
  }

 private:
  void remove(int id) {
    std::erase_if(callees_, [&](auto const id_callee_pair) {
      return id_callee_pair.first == id;
    });
  }

  int next_id_ = 0;
  using entry = std::pair<int, callee>;
  std::vector<entry> callees_;
};

// --------------------------------------------------------------------------------
// factory

class unkonwn_factory_key_error : public error {
  using error::error;
};

template <template <typename> typename Any, typename Key, typename... Args>
class factory {
  using constructor_t = std::function<Any<unique>(Args...)>;
  std::map<Key, constructor_t> function_map_;

 public:
  auto register_(Key const& key, auto const& construct) {
    function_map_[key] = construct;
    return nullptr;
  }
  Any<unique> construct(auto key, Args&&... args) {
    if (auto found = function_map_.find(key); found != function_map_.end())
      return found->second(std::forward<Args>(args)...);
    if constexpr (std::same_as<Key, std::string>) {
      throw unkonwn_factory_key_error{key};
    } else {
      throw unkonwn_factory_key_error{std::to_string(key)};
    }
  };
};

// --------------------------------------------------------------------------------
// extension member

#ifdef ANY_DLL_MODE
template <typename InObject>
std::size_t& members_count();
#else
template <typename InObject>
std::size_t& members_count() {
  static std::size_t count = 0;
  return count;
}
#endif

template <typename InObject>
struct members {
  members() : table_(members_count<InObject>()) {}
  std::vector<value> table_;
  template <typename Member, typename Arg>
  void set(Member member, Arg&& arg) {
    using value_t = typename Member::value_t;
    table_[member.index] = make_value<value_t>(std::forward<Arg>(arg));
  }
  template <typename Member>
  typename Member::value_t const* get(Member member) const {
    const auto& value = table_[member.index];
    if (!value) return {};
    return unchecked_unerase_cast<typename Member::value_t>(value);
  }
  template <typename Member>
  typename Member::value_t* get(Member member) {
    auto& value = table_[member.index];
    if (!value) return {};
    return unchecked_unerase_cast<typename Member::value_t>(value);
  }
  template <typename Member>
  typename Member::value_t& operator[](Member member) {
    if (auto value = get(member)) {
      return *value;
    }
    using value_t = typename Member::value_t;
    set(member, value_t());
    return *get(member);
  }
};

template <typename InObject, typename ValueType>
struct member {
  using object_t = InObject;
  using value_t = ValueType;
  std::size_t index = members_count<InObject>()++;
};

// --------------------------------------------------------------------------------
// dispatch

#ifdef ANY_DLL_MODE
template <typename AnyVTable>
std::size_t& dispatchs_count();
#else
template <typename AnyVTable>
std::size_t& dispatchs_count() {
  static std::size_t count = 0;
  return count;
}
#endif

template <is_any Any>
struct virtual_ {
  using type = Any;
};

template <typename Arg>
struct translate_erased_function_param {
  using type = Arg;
};
template <is_any Any>
struct translate_erased_function_param<virtual_<Any>> {
  using type = typename Any::void_t;
};

template <typename RET, typename... Args>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<Args>::type...);
};

template <std::size_t I, typename First, typename... Args>
auto arg_n(First first, Args... args) {
  if constexpr (I == 0) {
    return first;
  } else {
    return arg_n<I - 1>(std::forward<Args>(args)...);
  }
}

template <std::size_t COUNT, typename... Args>
constexpr std::size_t dispatch_dimension_count = COUNT;
template <std::size_t COUNT, is_any Any, typename... Args>
constexpr std::size_t dispatch_dimension_count<COUNT, virtual_<Any>, Args...> =
    dispatch_dimension_count<COUNT + 1, Args...>;

template <typename R, typename... Classes>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... Args>
  struct striped_virtuals {
    static R function(Classes&... classes, Args... args) {
      return FUNCTOR{}(classes..., args...);
    };
  };
  template <typename FUNCTOR, is_any Any, typename... Args>
  struct striped_virtuals<FUNCTOR, virtual_<Any>, Args...>
      : striped_virtuals<FUNCTOR, Args...> {};

  template <typename... Args>
  static auto instance(
      auto functor)  // if functor is a templated operator() from a
                     // stateless function object, instantiate it now!;
  {
    using functor_t = decltype(functor);
    if constexpr (std::is_pointer_v<functor_t>) {
      return functor;
    } else {
      return striped_virtuals<functor_t, Args...>::function;
    }
  }
};

template <typename... DispatchArgs>
struct args_to_tuple {
  template <typename T, typename... ActualArgs>
  auto operator()(T&& dispatch_args, ActualArgs&&... actual_args) {
    return std::tuple_cat(
        std::forward<T>(dispatch_args),
        std::make_tuple(std::forward<ActualArgs>(actual_args)...));
  }
};
template <is_any Any, typename... DispatchArgs>
struct args_to_tuple<virtual_<Any>, DispatchArgs...> {
  template <typename T, typename ACTUAL_ARG, typename... ActualArgs>
  auto operator()(T&& dispatch_args, ACTUAL_ARG&& dispatch_arg,
                  ActualArgs&&... actual_args) {
    return args_to_tuple<DispatchArgs...>{}(
        std::tuple_cat(std::forward<T>(dispatch_args),
                       std::make_tuple(get_void_data_ptr(dispatch_arg))),
        std::forward<ActualArgs>(actual_args)...);
  }
};

template <std::size_t FirstN, typename... Ts>
auto get_tuple_head(std::tuple<Ts...> from) {
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(from)...);
  }(std::make_index_sequence<FirstN>{});
}
template <std::size_t FromN, typename... Ts>
auto get_tuple_tail(std::tuple<Ts...> from) {
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(from)...);
  }(std::make_index_sequence<std::tuple_size_v<std::tuple<Ts...>> - FromN>{});
}

template <typename R, typename... FArgs>
struct dispatch_function;
template <typename R, typename... FArgs>
struct dispatch_function<R, std::tuple<FArgs...>> {
  using type = R (*)(FArgs...);
};

template <typename R, typename... OuterArgs>
struct dispatch_default {
  template <is_any... Anys>
  struct inner {
    template <typename... Args>
    struct implemenation {
      struct type {
        using function_t = hook<R(Anys const&..., Args...)>;
        static auto function() {
          return []([[maybe_unused]] auto super,
                    [[maybe_unused]] Anys const&... anys,
                    [[maybe_unused]] Args... args) -> R {
            if constexpr (std::same_as<R, void>) {
              return;
            } else {
              return R{};
            }
          };
        }
      };
    };
    template <is_any Any, typename... Args>
    struct implemenation<virtual_<Any>, Args...> : implemenation<Args...> {};
    template <typename... Args>
    using type = typename implemenation<Args...>::type;
  };

  template <typename... Args>
  struct outer {
    template <is_any... Anys>
    using type = inner<Anys...>::template type<Args...>;
  };
  template <is_any Any, typename... Args>
  struct outer<virtual_<Any>, Args...> {
    template <is_any... Anys>
    using type = outer<Args...>::template type<Any, Anys...>;
  };

  using type = outer<OuterArgs...>::template type<>;
};

template <typename F, typename... Args>
struct dispatch_matrix {
  using type = F;
};
template <typename DispatchMatrix, is_any Any, typename... Args>
struct dispatch_matrix<DispatchMatrix, virtual_<Any>, Args...> {
  using type =
      typename dispatch_matrix<std::vector<DispatchMatrix>, Args...>::type;
};

template <typename R, typename... Args>
struct dispatch;
template <typename R, typename... Args>
struct dispatch<R(Args...)> {
  using erased_function_t =
      typename translate_erased_function<R, Args...>::type;

  static constexpr std::size_t dimension_count =
      dispatch_dimension_count<0, Args...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, Args...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using dispatch_default_t = typename dispatch_default<R, Args...>::type;
  dispatch_default_t::function_t dispatch_default_hook_;
  dispatch_default_t::function_t::connection default_connection_ =
      dispatch_default_hook_.insert(dispatch_default_t::function());

  enum class kind { single, multiple };
  template <kind Kind, std::size_t Dimension, typename... DispatchArgs>
  struct dispatch_access;

  template <typename... DispatchArgs>
  struct dispatch_access<kind::multiple, dimension_count, DispatchArgs...> {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename F, typename ArgsTuple>
    std::optional<R> invoke(F const& target, ArgsTuple&& dispatch_args_tuple,
                            auto&&...) const {
      if (!target) return {};
      auto typed_target = reinterpret_cast<
          typename dispatch_function<R, std::decay_t<ArgsTuple>>::type>(target);
      return std::apply(typed_target,
                        std::forward<ArgsTuple>(dispatch_args_tuple));
    }
  };

  template <std::size_t Dimension, is_any Any, typename... DispatchArgs>
  struct dispatch_access<kind::multiple, Dimension, virtual_<Any>,
                         DispatchArgs...>
      : dispatch_access<kind::multiple, Dimension + 1, DispatchArgs...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    using next_t =
        dispatch_access<kind::multiple, Dimension + 1, DispatchArgs...>;

    // index 0 is for the 'wildcard' functions
    std::size_t index_ = 1 + dispatchs_count<v_table_t>()++;
    std::size_t dispatch_dimension_size_ = 1;

    template <typename Class>
    std::size_t get_dispatch_index() {
      if constexpr (std::same_as<Any, Class>) {
        return 0;
      } else {
        auto dispatch_table = dispatch_table_instance<v_table_t, Class>();
        if (auto index = get_multi_dispatch_index_at(dispatch_table, index_))
          return index;
        else
          set_multi_dispatch_index_at(dispatch_table, index_,
                                      dispatch_dimension_size_);
        return dispatch_dimension_size_++;
      }
    }

    template <typename Class, typename... Classes>
    auto define(auto fp, auto& matrix) {
      auto dispatch_index = get_dispatch_index<Class>();
      if (matrix.size() <= dispatch_index) matrix.resize(dispatch_index + 1);
      return next_t::template define<Classes...>(fp, matrix[dispatch_index]);
    }

    template <typename DispatchMatrix, typename ArgsTuple,
              typename... ActualArgs>
    std::optional<R> invoke(DispatchMatrix const& target,
                            ArgsTuple&& dispatch_args_tuple, Any const& any,
                            ActualArgs&&... actual_args) const {
      auto dispatch_table = get_v_table(any)->dispatch_table;
      auto dispatch_dim = get_multi_dispatch_index_at(dispatch_table, index_);
      if (dispatch_dim && target.size() > dispatch_dim)
        if (auto found =
                next_t::invoke(target[dispatch_dim],
                               std::forward<ArgsTuple>(dispatch_args_tuple),
                               std::forward<ActualArgs>(actual_args)...))
          return found;

      return next_t::invoke(
          target[0],
          std::tuple_cat(get_tuple_head<Dimension>(dispatch_args_tuple),
                         std::make_tuple(&any),
                         get_tuple_tail<Dimension + 1>(dispatch_args_tuple)),
          std::forward<ActualArgs>(actual_args)...);
    }
  };

  template <is_any Any, typename... AccessArgs>
  struct dispatch_access<kind::single, 0, virtual_<Any>, AccessArgs...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    std::size_t index_ = dispatchs_count<v_table_t>()++;

    template <typename CLASS>
    auto define(auto fp, auto&) {
      auto v_table = dispatch_table_instance<v_table_t, CLASS>();
      insert_function(v_table, index_, fp);
      return fp;
    }

    template <typename... Other>
    R invoke(dispatch_default_t::function_t const& default_, Any const& any,
             Other&&... other) const {
      auto v_table = get_v_table(any)->dispatch_table;
      auto target = get_function(v_table, index_);
      if (!target)
        return std::invoke(default_, any, std::forward<Other>(other)...);
      auto erased_function = reinterpret_cast<erased_function_t>(target);
      return std::invoke(erased_function, get_void_data_ptr(any),
                         std::forward<Other>(other)...);
    }
  };

  static const constexpr kind dispatch_kind =
      (dimension_count > 1) ? kind::multiple : kind::single;
  dispatch_access<dispatch_kind, 0, Args...> dispatch_access_;

 public:
  template <typename... Classes>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, Classes...>::template instance<Args...>(f);
    return dispatch_access_.template define<Classes...>(fp, dispatch_matrix_);
  };
  template <typename... ActualArgs>
  auto operator()(ActualArgs&&... actual_args) const {
    if constexpr (dispatch_kind == kind::multiple) {
      auto dispatch_args_tuple = args_to_tuple<Args...>{}(
          std::tuple<>{}, std::forward<ActualArgs>(actual_args)...);
      return *dispatch_access_
                  .invoke(dispatch_matrix_, dispatch_args_tuple,
                          std::forward<ActualArgs>(actual_args)...)
                  .or_else([&]() -> std::optional<R> {
                    return std::invoke(
                        dispatch_default_hook_,
                        std::forward<ActualArgs>(actual_args)...);
                  });
    } else {
      return dispatch_access_.invoke(dispatch_default_hook_,
                                     std::forward<ActualArgs>(actual_args)...);
    }
  }
  auto& get_dispatch_default_hook() { return dispatch_default_hook_; };
};

}  // namespace anyxx

#define ANY_MERGE_(a, b) a##b
#define ANY_LABEL_(a) ANY_MERGE_(unique_name_, a)
#define ANY_UNIQUE_NAME_ ANY_LABEL_(__COUNTER__)
#define ANY_UNIQUE_NAME ANY_UNIQUE_NAME_
#define __ ANY_UNIQUE_NAME_

#ifdef ANY_DLL_MODE

#define ANY_META_CLASS_FWD(export_, ...)                         \
  template <>                                                    \
  export_ const std::type_info& anyxx::typeid_of<__VA_ARGS__>(); \
  template <>                                                    \
  export_ anyxx::meta_data& anyxx::get_meta_data<__VA_ARGS__>();

#define ANY_META_CLASS(...)                                             \
  template <>                                                           \
  const std::type_info& anyxx::typeid_of<__VA_ARGS__>() {               \
    return typeid(__VA_ARGS__);                                         \
  }                                                                     \
  template <>                                                           \
  anyxx::meta_data& anyxx::get_meta_data<std::decay_t<__VA_ARGS__>>() { \
    return runtime_implementation<__VA_ARGS__>();                       \
  }

#else

#define ANY_META_CLASS_FWD(...)
#define ANY_META_CLASS(...)

#endif

#define ANY_META_CLASS_STATIC(...)  \
  ANY_META_CLASS_FWD(, __VA_ARGS__) \
  ANY_META_CLASS(__VA_ARGS__)

#ifdef ANY_DLL_MODE

#define ANY_MEMBERS_COUNT_FWD(export_, ns_, c_)  \
  namespace ns_ {                                \
  struct c_;                                     \
  }                                              \
  namespace anyxx {                              \
  template <>                                    \
  export_ std::size_t& members_count<ns_::c_>(); \
  }

#define ANY_MEMBERS_COUNT_IMPL(ns_, c_)          \
  template <>                                    \
  std::size_t& anyxx::members_count<ns_::c_>() { \
    static std::size_t count = 0;                \
    return count;                                \
  }

#define ANY_MEMBER_FWD(export_, object_, member_, type_)           \
  export_ anyxx::member<object_, type_>& _inintialize_##member_(); \
  inline const anyxx::member<object_, type_>& member_ =            \
      _inintialize_##member_();

#define ANY_MEMBER_IMPL(ns_, object_, member_, type_)            \
  anyxx::member<object_, type_>& ns_::_inintialize_##member_() { \
    static anyxx::member<object_, type_> instance;               \
    return instance;                                             \
  }

#else

#define ANY_MEMBERS_COUNT_FWD(...)
#define ANY_MEMBERS_COUNT_IMPL(...)
#define ANY_MEMBER_FWD(...)
#define ANY_MEMBER_IMPL(...)

#endif

#ifdef ANY_DLL_MODE

#define ANY_DISPATCH_COUNT_FWD(export_, ns_, c_)   \
  namespace ns_ {                                  \
  struct c_;                                       \
  }                                                \
  namespace anyxx {                                \
  template <>                                      \
  export_ std::size_t& dispatchs_count<ns_::c_>(); \
  }

#define ANY_DISPATCH_COUNT(ns_, c_)                \
  template <>                                      \
  std::size_t& anyxx::dispatchs_count<ns_::c_>() { \
    static std::size_t count = 0;                  \
    return count;                                  \
  }

#define ANY_DISPATCH_FOR_FWD(export_, class_, interface_namespace_) \
  namespace anyxx {                                                 \
  template <>                                                       \
  export_ anyxx::dispatch_table_t*                                  \
  anyxx::dispatch_table_instance<interface_##_v_table, class_>();   \
  }

#define ANY_DISPATCH_FOR(class_, interface_namespace_, interface_)      \
  template <>                                                           \
  anyxx::dispatch_table_t*                                              \
  anyxx::dispatch_table_instance<interface_##_v_table, class_>() {      \
    return dispatch_table_instance_implementation<interface_##_v_table, \
                                                  class_>();            \
  }

#else

#define ANY_DISPATCH_COUNT_FWD(...)
#define ANY_DISPATCH_COUNT(...)
#define ANY_DISPATCH_FOR_FWD(...)
#define ANY_DISPATCH_FOR(...)

#endif

// --------------------------------------------------------------------------------
// any meta class, derived from this gem:
// https://github.com/AlexCodesApps/dynamic_interface

#define _detail_EXPAND(...) \
  _detail_EXPAND4(          \
      _detail_EXPAND4(_detail_EXPAND4(_detail_EXPAND4(__VA_ARGS__))))
#define _detail_EXPAND4(...) \
  _detail_EXPAND3(           \
      _detail_EXPAND3(_detail_EXPAND3(_detail_EXPAND3(__VA_ARGS__))))
#define _detail_EXPAND3(...) \
  _detail_EXPAND2(           \
      _detail_EXPAND2(_detail_EXPAND2(_detail_EXPAND2(__VA_ARGS__))))
#define _detail_EXPAND2(...) \
  _detail_EXPAND1(           \
      _detail_EXPAND1(_detail_EXPAND1(_detail_EXPAND1(__VA_ARGS__))))
#define _detail_EXPAND1(...) __VA_ARGS__

#define _detail_EXPAND_(...) \
  _detail_EXPAND_4(          \
      _detail_EXPAND_4(_detail_EXPAND_4(_detail_EXPAND_4(__VA_ARGS__))))
#define _detail_EXPAND_4(...) \
  _detail_EXPAND_3(           \
      _detail_EXPAND_3(_detail_EXPAND_3(_detail_EXPAND_3(__VA_ARGS__))))
#define _detail_EXPAND_3(...) \
  _detail_EXPAND_2(           \
      _detail_EXPAND_2(_detail_EXPAND_2(_detail_EXPAND_2(__VA_ARGS__))))
#define _detail_EXPAND_2(...) \
  _detail_EXPAND_1(           \
      _detail_EXPAND_1(_detail_EXPAND_1(_detail_EXPAND_1(__VA_ARGS__))))
#define _detail_EXPAND_1(...) __VA_ARGS__
#define _detail_PARENS ()
#define _detail_APPLY(macro, args) macro args
#define _detail_REMOVE_PARENS(l) _detail_APPLY(_detail_EXPAND_1, l)
#define _detail_foreach_macro_h(macro, a, ...) \
  macro(a)                                     \
      __VA_OPT__(_detail_foreach_macro_a _detail_PARENS(macro, __VA_ARGS__))
#define _detail_foreach_macro_a() _detail_foreach_macro_h
#define _detail_foreach_macro(macro, ...) \
  _detail_EXPAND(_detail_foreach_macro_h(macro, __VA_ARGS__))
#define _detail_map_macro_h(macro, a, ...) \
  macro(a) __VA_OPT__(, _detail_map_macro_a _detail_PARENS(macro, __VA_ARGS__))
#define _detail_map_macro(macro, ...) \
  _detail_EXPAND(_detail_map_macro_h(macro, __VA_ARGS__))
#define _detail_map_macro_a() _detail_map_macro_h
#define _detail_CONCAT_H(a, b) a##b
#define _detail_CONCAT(a, b) _detail_CONCAT_H(a, b)
#define _detail_PARAM_LIST_H(b, c, f, ...)              \
  std::forward<decltype(c)>(c)                          \
      __VA_OPT__(, _detail_PARAM_LIST_A _detail_PARENS( \
                       b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_A() _detail_PARAM_LIST_H
#define _detail_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_PARAM_LIST_H(__VA_ARGS__))
#define _detail_PARAM_LIST_2H(b, c, f, ...)                               \
  [[maybe_unused]] f c __VA_OPT__(, _detail_PARAM_LIST_2A _detail_PARENS( \
                                        b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_2A() _detail_PARAM_LIST_2H
#define _detail_PARAM_LIST2(...) \
  _detail_EXPAND_(_detail_PARAM_LIST_2H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_ANYXX_TYPENAME_PARAM_H(t) _detail_ANYXX_TYPENAME_PARAM t
#define _detail_ANYXX_TYPENAME_PARAM(t) , typename t
#define _detail_ANYXX_TYPENAME_PARAM_LIST(head, ...) \
  typename _strip_braces head __VA_OPT__(            \
      _detail_foreach_macro(_detail_ANYXX_TYPENAME_PARAM_H, __VA_ARGS__))

#define _detail_ANYXX_TEMPLATE_ARG_H(t) _detail_ANYXX_TEMPLATE_ARG t
#define _detail_ANYXX_TEMPLATE_ARG(t) , t
#define _detail_ANYXX_TEMPLATE_ARGS(head, ...) \
  _strip_braces head __VA_OPT__(               \
      _detail_foreach_macro(_detail_ANYXX_TEMPLATE_ARG_H, __VA_ARGS__))

#define _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS_H(...) \
  __VA_OPT__(<_detail_ANYXX_TEMPLATE_ARGS(__VA_ARGS__)>)
#define _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(t) \
  _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS_H t

#define _detail_LEAD_COMMA_H(...) __VA_OPT__(, )
#define _detail_ANYXX_FPD_H(l) _detail_ANYXX_FUNCTION_PTR_DECL l
#define _detail_ANYXX_MEMEBER_LIMP_H(l) _detail_ANYXX_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_ANYXX_METHOD_H(l) _detail_ANYXX_METHOD l

#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define _add_head_1(a, ...) a, __VA_ARGS__
#define _add_head(a, x) _add_head_1(a, _strip_braces x)
#define _strip_braces _strip_braces_1
#define _strip_braces_1(...) __VA_ARGS__

#define _typename _typename1
#define _typename1(t) t

#define _detail_ANYXX_TEMPLATE_FORMAL_ARG_H(l) \
  _detail_ANYXX_TEMPLATE_FORMAL_ARG l
#define _detail_ANYXX_TEMPLATE_FORMAL_ARG(_typename) , typename _typename
#define _detail_ANYXX_TEMPLATE_FORMAL_ARGS(...) \
  __VA_OPT__(_detail_ANYXX_TEMPLATE_FORMAL_ARGS1(__VA_ARGS__))
#define _detail_ANYXX_TEMPLATE_FORMAL_ARGS1(h, ...) \
  typename _typename h __VA_OPT__(                  \
      _detail_ANYXX_TEMPLATE_FORMAL_ARGS2((__VA_ARGS__)))
#define _detail_ANYXX_TEMPLATE_FORMAL_ARGS2(l)               \
  _detail_foreach_macro(_detail_ANYXX_TEMPLATE_FORMAL_ARG_H, \
                        _detail_EXPAND_LIST l)

#define _detail_ANYXX_V_TABLE_TEMPLATE_HEADER_H(...) \
  __VA_OPT__(template <_detail_ANYXX_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_ANYXX_V_TABLE_TEMPLATE_HEADER(t) \
  _detail_ANYXX_V_TABLE_TEMPLATE_HEADER_H t

#define _detail_ANYXX_INVOKE_TEMPLATE_PARAMS_H(...) __VA_OPT__(<__VA_ARGS__>)

#define _detail_ANYXX_INVOKE_TEMPLATE_PARAMS(t) \
  _detail_ANYXX_INVOKE_TEMPLATE_PARAMS_H t

#define _detail_ANYXX_EXPAND_WITH_LEADING_COMMA(...) __VA_OPT__(, ) __VA_ARGS__

#define _detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(t) \
  ErasedData _detail_ANYXX_EXPAND_WITH_LEADING_COMMA(_detail_REMOVE_PARENS(t))

#define _detail_ANYXX_MAP_LIMP_H(l) _detail_ANYXX_MAP_IMPL l
#define _detail_ANYXX_MAP_IMPL(overload, type, name, name_ext, exact_const,  \
                               const_, ...)                                  \
  auto name(T const_& x __VA_OPT__(                                          \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {                \
    return x.name_ext(__VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
  };

#define _detail_ANYXX_FUNCTION_PTR_DECL(overload, type, name, name_ext, \
                                        exact_const, const_, ...)       \
  type (*name)(void const_* __VA_OPT__(, __VA_ARGS__));

#define _detail_ANYXX_LAMBDA_TO_MEMEBER_IMPL(overload, type, name, name_ext, \
                                             exact_const, const_, ...)       \
  name = [](void const_* _vp __VA_OPT__(                                     \
             , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {         \
    return concept_map{}.name(                                               \
        *anyxx::unchecked_unerase_cast<Concrete>(_vp) __VA_OPT__(, )         \
            __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));           \
  };

#define _detail_ANYXX_METHOD(overload, type, name, name_ext, exact_const, \
                             const_, ...)                                 \
  overload type name_ext(                                                 \
      __VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const_       \
    requires(::anyxx::const_correct_call_for_erased_data<                 \
             void const_*, erased_data_t, exact_const>)                   \
  {                                                                       \
    return get_v_table_ptr()->name(                                       \
        anyxx::get_void_data_ptr(base_t::erased_data_)                    \
            __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));      \
  }

#define _detail_ANYXX_MAP_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_V_TABLE_FUNCTION_PTRS(...)        \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_FPD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_V_TABLE_LAMBDAS(...)                       \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MEMEBER_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_METHODS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_METHOD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define ANY_META_FUNCTION(tpl1, tpl2, tpl3, tpl4, tpl, n, BASE, btpl, l)      \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                          \
  struct n;                                                                   \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                          \
  struct n##_default_concept_map {                                            \
    _detail_ANYXX_MAP_FUNCTIONS(l)                                            \
  };                                                                          \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl2)>                          \
  struct n##_concept_map                                                      \
      : n##_default_concept_map<_detail_ANYXX_TEMPLATE_ARGS(tpl2)> {};        \
                                                                              \
  _detail_ANYXX_V_TABLE_TEMPLATE_HEADER(tpl) struct n##_v_table;              \
                                                                              \
  _detail_ANYXX_V_TABLE_TEMPLATE_HEADER(tpl) struct n##_v_table               \
      : BASE##_v_table                                                        \
        _detail_ANYXX_INVOKE_TEMPLATE_PARAMS(btpl),                           \
        anyxx::dispatch_holder<anyxx::has_dispatchs<n>, n> {                  \
    using v_table_base_t =                                                    \
        BASE##_v_table _detail_ANYXX_INVOKE_TEMPLATE_PARAMS(btpl);            \
    using v_table_t = n##_v_table;                                            \
    using own_dispatch_holder_t =                                             \
        typename anyxx::dispatch_holder<anyxx::has_dispatchs<n>, n>;          \
                                                                              \
    static bool static_is_derived_from(const std::type_info& from) {          \
      return typeid(v_table_t) == from                                        \
                 ? true                                                       \
                 : v_table_base_t::static_is_derived_from(from);              \
    }                                                                         \
                                                                              \
    _detail_ANYXX_V_TABLE_FUNCTION_PTRS(l);                                   \
                                                                              \
    static constexpr bool dispatchs_enabled = anyxx::has_dispatchs<n>;        \
                                                                              \
    template <typename Concrete>                                              \
    explicit(false) n##_v_table(std::in_place_type_t<Concrete> concrete)      \
        : v_table_base_t(concrete) {                                          \
      using concept_map = n##_concept_map<_detail_ANYXX_TEMPLATE_ARGS(tpl3)>; \
                                                                              \
      _detail_ANYXX_V_TABLE_LAMBDAS(l);                                       \
                                                                              \
      if constexpr (dispatchs_enabled) {                                      \
        own_dispatch_holder_t::set_dispatch_table(                            \
            ::anyxx::dispatch_table_instance<n##_v_table, Concrete>());       \
      }                                                                       \
                                                                              \
      ::anyxx::set_is_derived_from<v_table_t>(this);                          \
    };                                                                        \
                                                                              \
    template <typename Concrete>                                              \
    static auto imlpementation() {                                            \
      return anyxx::v_table_instance_implementaion<v_table_t, Concrete>();    \
    }                                                                         \
  };                                                                          \
                                                                              \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(tpl1)>                          \
  struct n : BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)> {            \
    using erased_data_t = ErasedData;                                         \
    using base_t = BASE<_detail_ANYXX_BASE_TEMPLATE_ACTUAL_ARGS(btpl)>;       \
    using v_table_base_t = base_t::v_table_t;                                 \
    using v_table_t =                                                         \
                                                                              \
        n##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(tpl);          \
                                                                              \
    template <typename Concrete>                                              \
    static auto v_table_imlpementation() {                                    \
      static_assert(!anyxx::is_any<Concrete>);                                \
      return v_table_t::template imlpementation<Concrete>();                  \
    }                                                                         \
                                                                              \
    using base_t::erased_data_;                                               \
    using base_t::v_table_;                                                   \
                                                                              \
    n(erased_data_t erased_data, v_table_t* v_table)                          \
        : base_t(std::move(erased_data), v_table) {}                          \
    template <typename ConstructedWith>                                       \
    explicit(false) n(ConstructedWith&& v)                                    \
      requires anyxx::constructibile_for<ConstructedWith, ErasedData>         \
        : base_t(std::forward<ConstructedWith>(v)) {                          \
      v_table_ = v_table_t::template imlpementation<                          \
          anyxx::unerased<ErasedData, ConstructedWith>>();                    \
    }                                                                         \
    template <typename V>                                                     \
    n(std::in_place_t, V&& v) : base_t(std::in_place, std::forward<V>(v)) {   \
      v_table_ = v_table_t::template imlpementation<                          \
          anyxx::unerased<ErasedData, V>>();                                  \
    }                                                                         \
    template <typename T, typename... Args>                                   \
    explicit(false) n(std::in_place_type_t<T>, Args&&... args)                \
        : base_t(std::in_place_type<T>, std::forward<Args>(args)...) {        \
      v_table_ = v_table_t::template imlpementation<                          \
          anyxx::unerased<ErasedData, T>>();                                  \
    }                                                                         \
    template <typename Other>                                                 \
    explicit(false) n(const Other& other)                                     \
      requires(std::derived_from<typename Other::v_table_t, v_table_t> &&     \
               anyxx::borrowable_from<erased_data_t,                          \
                                      typename Other::erased_data_t>)         \
        : base_t(other) {}                                                    \
    template <anyxx::is_any Other>                                            \
    explicit(false) n(Other&& other) noexcept                                 \
      requires(                                                               \
          std::derived_from<typename Other::v_table_t, v_table_t> &&          \
          anyxx::moveable_from<erased_data_t, typename Other::erased_data_t>) \
        : base_t(std::forward<Other>(other)) {}                               \
                                                                              \
    auto get_v_table_ptr(this auto& self) {                                   \
      return static_cast<v_table_t*>(self.v_table_);                          \
    }                                                                         \
    _detail_ANYXX_METHODS(l)                                                  \
                                                                              \
        auto const*                                                           \
        operator->() const {                                                  \
      return this;                                                            \
    }                                                                         \
                                                                              \
    ~n() = default;                                                           \
    n() = default;                                                            \
    n(n const&) = default;                                                    \
    n(n&&) = default;                                                         \
    n& operator=(n const&) = default;                                         \
    n& operator=(n&&) = default;                                              \
    template <anyxx::is_erased_data Other>                                    \
    friend class anyxx::any_base;                                             \
    template <anyxx::is_any To, anyxx::is_any From>                           \
    friend To anyxx::unchecked_downcast_to(From from)                         \
      requires(std::derived_from<To, From>);                                  \
    template <anyxx::is_erased_data Other>                                    \
    using type_for = n<_detail_ANYXX_TEMPLATE_ARGS(tpl4)>;                    \
  };

#define ANY_(n, BASE, l) \
  ANY_META_FUNCTION((ErasedData), (T), (Concrete), (Other), (), n, BASE, (), l)

#define ANY(n, ...) ANY_(n, ::anyxx::any_base, __VA_ARGS__)

#define ANY_TEMPLATE_(t, n, BASE, btpl, l)                                 \
  ANY_META_FUNCTION(_add_head((ErasedData), t), _add_head((T), t),         \
                    _add_head((Concrete), t), _add_head((Other), t), t, n, \
                    BASE, btpl, l)

#define ANY_TEMPLATE(t, n, l) ANY_TEMPLATE_(t, n, ::anyxx::any_base, (), l)

#define ANY_METHOD_(...) (__VA_ARGS__)

#define ANY_METHOD(ret, name, params, const_) \
  ANY_METHOD_(, ret, name, name, false, const_, _detail_EXPAND params)

#define ANY_OVERLOAD(name) using base_t::name;

#define ANY_METHOD_OVERLOAD(ret, name, params, const_)            \
  ANY_METHOD_(ANY_OVERLOAD(name), ret, name, name, false, const_, \
              _detail_EXPAND params)

#define ANY_OP(ret, op, params, const_)                                       \
  ANY_METHOD_(, ret, _detail_CONCAT(__op__, __COUNTER__), operator op, false, \
              const_, _detail_EXPAND params)

#define ANY_OP_EXACT(ret, op, params, const_)                                \
  ANY_METHOD_(, ret, _detail_CONCAT(__op__, __COUNTER__), operator op, true, \
              const_, _detail_EXPAND params)

#define ANY_OP_EXACT_OVERLOAD(ret, op, params, const_)                        \
  ANY_METHOD_(ANY_OVERLOAD(operator op), ret,                                 \
              _detail_CONCAT(__op__, __COUNTER__), operator op, true, const_, \
              _detail_EXPAND params)

#define ANY_FORWARD(interface_namespace, interface_name) \
  namespace interface_namespace {                        \
  template <typename ErasedData>                         \
  struct interface_name;                                 \
  struct interface_name##_v_table;                       \
  }

#define ANY_REGISTER_MODEL(class_, interface_)                          \
  namespace {                                                           \
  static auto __ =                                                      \
      anyxx::bind_v_table_to_meta_data<interface_##_v_table, class_>(); \
  }

#ifdef ANY_DLL_MODE

#define ANY_MODEL_FWD(export_, class_, interface_namespace_, interface_)     \
  ANY_FORWARD(interface_namespace_, interface_)                              \
  namespace anyxx {                                                          \
  template <>                                                                \
  export_ interface_namespace_::interface_##_v_table*                        \
  v_table_instance_implementaion<interface_namespace_::interface_##_v_table, \
                                 class_>();                                  \
  }

#define ANY_MODEL(class_, interface_namespace_, interface_)    \
  template <>                                                  \
  interface_namespace_::interface_##_v_table*                  \
  anyxx::v_table_instance_implementaion<                       \
      interface_namespace_::interface_##_v_table, class_>() {  \
    static interface_namespace_::interface_##_v_table v_table{ \
        std::in_place_type<class_>};                           \
    return &v_table;                                           \
  }                                                            \
  ANY_REGISTER_MODEL(class_, interface_namespace_::interface_)

#else

#define ANY_MODEL_FWD(...)
#define ANY_MODEL(...)

#endif

#define ANY_MODEL_STATIC(class_, interface_, interface_namespace_) \
  ANY_MODEL_FWD(, class_, interface_, interface_namespace_)        \
  ANY_MODEL(, class_, interface_, interface_namespace_)

#define ANY_HAS_DISPATCH(interface_namespace, interface_name)               \
  ANY_FORWARD(interface_namespace, interface_name)                          \
                                                                            \
  namespace anyxx {                                                         \
  template <>                                                               \
  constexpr bool has_dispatchs<interface_namespace::interface_name> = true; \
  }
