#pragma once

//
// for Microsoft C++, you must enable the C-Preprocessor with this flag:
// /Zc:preprocessor (see CMakeLists.txt for example)
//

#include <cassert>
#include <concepts>
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

namespace anyxx {

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class type_mismatch_error : error {
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
U const* unerase_cast(ErasedData const* o, meta_data const& meta) {
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}
template <typename U, is_erased_data ErasedData>
U* unerase_cast(ErasedData const* o, meta_data const& meta)
  requires(!is_const_data<ErasedData>)
{
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
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

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  static Voidness value(const auto& ptr) { return ptr; }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

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

  static void const* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename ConstructedWith>
  using unerased = std::decay_t<typename ConstructedWith::element_type>;

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

  static void const* value(const auto& ptr) { return nullptr; }
  static bool has_value(const auto& ptr) { return !ptr.expired(); }

  template <typename ConstructedWith>
  using unerased = std::decay_t<typename ConstructedWith::element_type>;

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
auto make_unique(Args&&... args) {
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

  static void* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename ConstructedWith>
  using unerased = typename ConstructedWith::element_type;

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

template <typename T, typename... Args>
auto make_void_value(Args&&... args) {
  return make_value<T>(std::forward<Args>(args)...);
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

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename ConstructedWith>
  static auto erase(ConstructedWith&& v) {
    return anyxx::make_value<std::decay_t<ConstructedWith>>(
        std::forward<ConstructedWith>(v));
  }
};

static_assert(is_erased_data<value>);

// --------------------------------------------------------------------------------
// meta data

class meta_data;
struct any_base_v_table;

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data{std::in_place_type<TYPE>};
  return meta_data;
}

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
  any_base_v_table(std::in_place_type_t<Concrete> concrete);

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(any_base_v_table) == from;
  }

  meta_data* meta_data = nullptr;

  bool (*_is_derived_from)(const std::type_info&);
};

inline bool is_derived_from(const std::type_info& from,
                            any_base_v_table const* any_base_v_table) {
  return any_base_v_table->_is_derived_from(from);
}

using dispatch_table_function_t = void (*)();
using dispatch_table_dispatch_index_t = std::size_t;
using dispatch_table_entry_t =
    std::variant<std::monostate, dispatch_table_dispatch_index_t,
                 dispatch_table_function_t>;
using dispatch_table_t = std::vector<dispatch_table_entry_t>;

void insert_function(dispatch_table_t* v_table, std::size_t index, auto fp) {
  if (v_table->size() <= index) v_table->resize(index + 1);
  v_table->at(index) = reinterpret_cast<dispatch_table_function_t>(fp);
}
inline dispatch_table_function_t get_function(dispatch_table_t* v_table,
                                              std::size_t index) {
  if (v_table->size() <= index) return {};
  if (auto f = std::get_if<dispatch_table_function_t>(&v_table->at(index)))
    return *f;
  else
    return {};
}

inline std::optional<std::size_t> get_multi_dispatch_index_at(
    dispatch_table_t* v_table, std::size_t index) {
  if (v_table->size() <= index) return {};
  if (auto dispatch_table_dispatch_index =
          std::get_if<dispatch_table_dispatch_index_t>(&v_table->at(index)))
    return *dispatch_table_dispatch_index;
  return {};
}
inline void set_multi_dispatch_index_at(
    dispatch_table_t* v_table, std::size_t index_multi_dispatch,
    dispatch_table_dispatch_index_t
        dispatch_index_of_class_in_dispatch_matrix) {
  if (v_table->size() <= index_multi_dispatch)
    v_table->resize(index_multi_dispatch + 1);
  v_table->at(index_multi_dispatch) =
      dispatch_index_of_class_in_dispatch_matrix;
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
      : type_info_(typeid_of<CLASS>()), copy_construct_(+[](const_void from) {
          return erased<unique>(
              std::make_unique<CLASS>(*static_cast<CLASS const*>(from)));
        }) {}

  constexpr const std::type_info& get_type_info() const { return type_info_; }
  auto copy_construct(const_void from) const { return copy_construct_(from); }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  std::expected<any_base_v_table*, cast_error> get_v_table(
      std::type_info const& typeid_) const {
    auto& i_table = get_i_table();
    for (auto v_table : i_table)
      if (is_derived_from(typeid_, v_table)) return v_table;
    return std::unexpected(cast_error{.to = typeid_, .from = get_type_info()});
  }
  auto register_v_table(any_base_v_table* v_table) {
    v_table->meta_data = this;
    if (std::ranges::find(get_i_table(), v_table) == get_i_table().end())
      i_table_.push_back(v_table);
    return v_table;
  }
};

template <typename Concrete>
any_base_v_table::any_base_v_table(std::in_place_type_t<Concrete> concrete)
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

template <typename V_TABLE, typename Concrete>
auto bind_v_table_to_meta_data() {
  auto v_table = V_TABLE::template imlpementation<Concrete>();
  get_meta_data<Concrete>().register_v_table(v_table);
  return v_table;
}

template <typename U>
bool type_match(meta_data const& meta_data) {
  return &meta_data.get_type_info() == &typeid_of<std::decay_t<U>>();
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
  auto operator()(const auto& from) {
    return mutable_observer{get_void_data_ptr(from)};
  }
};
template <is_erased_data From>
  requires(!is_weak_data<From>)
struct borrow_trait<const_observer, From> {
  auto operator()(const auto& from) {
    return const_observer{get_void_data_ptr(from)};
  }
};
template <>
struct borrow_trait<shared_const, shared_const> {
  auto operator()(const auto& from) { return from; }
};
template <>
struct borrow_trait<weak, weak> {
  auto operator()(const auto& from) { return from; }
};
template <>
struct borrow_trait<weak, shared_const> {
  auto operator()(const auto& from) { return weak{from}; }
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

template <is_erased_data To, is_erased_data From>
bool constexpr can_move_to_from = false;

template <typename To, typename From>
concept moveable_from =
    is_erased_data<From> && is_erased_data<To> && can_move_to_from<To, From>;

template <is_erased_data X>
bool constexpr can_move_to_from<X, X> = true;

template <>
bool constexpr can_move_to_from<shared_const, unique> = true;

template <>
bool constexpr can_move_to_from<weak, shared_const> = true;

template <voidness To, voidness From>
  requires const_correct_call<is_const_void<To>, is_const_void<From>,
                              is_weak_data<From>>
bool constexpr can_move_to_from<To, From> = true;

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
concept is_any_impl = requires(I i, I::erased_data_t ed) {
  typename I::void_t;
  typename I::erased_data_t;
  typename I::v_table_t;
  typename I::trait_t;
  { get_erased_data(i) };
};

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
    !is_any<ConstructedWith> &&
    !is_typed_any<std::remove_cvref_t<ConstructedWith>>;

template <is_erased_data ErasedData>
class any_base {
 public:
  using erased_data_t = ErasedData;
  using trait_t = trait<erased_data_t>;
  using void_t = typename trait_t::void_t;
  using v_table_t = any_base_v_table;

 protected:
  erased_data_t erased_data_ = {};
  v_table_t* v_table_ = nullptr;

  any_base() = default;
  any_base(erased_data_t erased_data, v_table_t* v_table)
      : erased_data_(std::move(erased_data)), v_table_(v_table) {}
  template <typename ConstructedWith>
  any_base(ConstructedWith&& constructed_with)
    requires constructibile_for<ConstructedWith, ErasedData>
      : erased_data_(erased<erased_data_t>(
            std::forward<ConstructedWith>(constructed_with))) {
    using t = unerased<ErasedData, ConstructedWith>;
  }

 public:
  template <is_any Otther>
  any_base(const Otther& other)
    requires(std::derived_from<typename Otther::v_table_t, v_table_t> &&
             borrowable_from<erased_data_t, typename Otther::erased_data_t>)
      : erased_data_(borrow_as<ErasedData>(other.erased_data_)),
        v_table_(get_v_table(other)) {}
  template <typename Otther>
  any_base(Otther&& other)
    requires(std::derived_from<typename Otther::v_table_t, v_table_t> &&
             moveable_from<erased_data_t, typename Otther::erased_data_t>)
      : erased_data_(move_to<ErasedData>(std::move(other.erased_data_))),
        v_table_(get_v_table(other)) {}
  template <typename Otther>
  any_base& operator=(Otther&& other)
    requires(std::derived_from<Otther::v_table_t, v_table_t>)
  {
    erased_data_ = move_to<ErasedData>(std::move(other.erased_data_));
    v_table_ = get_v_table(other);
    return *this;
  }
  any_base(const any_base&) = default;
  // any_base(any_base&) requires(std::is_copy_constructible_v<any_base>) =
  // default;
  any_base(any_base&& rhs) noexcept
      : erased_data_(std::move(rhs.erased_data_)), v_table_(rhs.v_table_) {}
  any_base& operator=(any_base const& other) = default;

  template <is_erased_data Otther>
  friend class any_base;

  template <is_erased_data ErasedData>
  friend inline auto& get_erased_data(any_base<ErasedData> const& any);
  template <is_erased_data ErasedData>
  friend inline auto move_erased_data(any_base<ErasedData>&& any);
  template <is_erased_data ErasedData>
  friend inline auto get_void_data_ptr(any_base<ErasedData> const& any);
  template <is_any I>
  friend inline auto get_v_table(I const& any);

  template <is_any To, is_any From>
  friend inline To unchecked_downcast_to(From from)
    requires(std::derived_from<To, From>);

 public:
  void operator()() const {}
  void* operator[](void*) const { return {}; }
};

template <typename V_TABLE, is_erased_data ErasedData>
struct interface_t;

template <typename V_TABLE, is_erased_data ErasedData>
using interface_for = typename interface_t<V_TABLE, ErasedData>::type;

template <is_erased_data ErasedData>
auto& get_erased_data(any_base<ErasedData> const& any) {
  return any.erased_data_;
}
template <is_erased_data ErasedData>
auto move_erased_data(any_base<ErasedData>&& any) {
  return std::move(any.erased_data_);
}
template <is_erased_data ErasedData>
auto get_void_data_ptr(any_base<ErasedData> const& any) {
  return get_void_data_ptr(get_erased_data(any));
}

template <is_any Any>
inline const auto& get_runtime(Any const& any) {
  return *get_v_table(any)->meta_data;
}

template <is_erased_data VV>
bool is_derived_from(const std::type_info& from, any_base<VV> const& any) {
  return get_v_table(any)->_is_derived_from(from);
}
template <is_any From, is_erased_data VV>
bool is_derived_from(any_base<VV> const& any) {
  return is_derived_from(typeid(From::v_table_t), any);
}

template <typename V_TABLE>
void set_is_derived_from(auto v_table) {
  v_table->_is_derived_from = +[](const std::type_info& from) {
    return V_TABLE::static_is_derived_from(from);
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
To unchecked_downcast_to(From from)
  requires(std::derived_from<To, From>)
{
  return To{std::move(from.erased_data_),
            unchecked_v_table_downcast_to<To>(from.v_table_)};
}

template <is_any To, is_any From>
std::optional<To> downcast_to(From from)
  requires(std::derived_from<To, From>)
{
  if (is_derived_from<To>(from))
    return {unchecked_downcast_to<To>(std::move(from))};
  return {};
}

template <typename U, is_any Any>
auto unchecked_unerase_cast(Any const& o) {
  return unchecked_unerase_cast<U>(get_erased_data(o));
}
template <typename U, is_any Any>
auto unerase_cast(Any const& o) {
  return unerase_cast<U>(get_erased_data(o), get_runtime(o));
}
template <typename U, is_any Any>
auto unerase_cast(Any const* o) {
  unerase_cast<U>(get_erased_data(*o), get_runtime(o));
  return nullptr;
}

#ifdef ANY_DLL_MODE
template <typename V_TABLE, typename Concrete>
V_TABLE* v_table_instance_implementaion();
#else
template <typename V_TABLE, typename Concrete>
V_TABLE* v_table_instance_implementaion() {
  static V_TABLE v_table{std::in_place_type<Concrete>};
  static auto __ = anyxx::get_meta_data<Concrete>().register_v_table(&v_table);
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
struct dispatch_holder<false, Any> {};
template <template <typename...> typename Any>
struct dispatch_holder<true, Any> {
  dispatch_table_t* dispatch_table = nullptr;
};

template <is_any TO_ANYPP>
auto query_v_table(const meta_data& meta_data)
    -> std::expected<typename TO_ANYPP::v_table_t*, anyxx::cast_error> {
  using v_table_t = typename TO_ANYPP::v_table_t;
  return meta_data.get_v_table(typeid(v_table_t)).transform([](auto v_table) {
    return static_cast<v_table_t*>(v_table);
  });
}

template <typename TO_ANYPP>
auto query_v_table(any_base_v_table* from) {
  return find_v_table<TO_ANYPP>(*from->meta_data);
}

// --------------------------------------------------------------------------------
// typed any

template <typename V, is_any Any>
struct typed_any : public Any {
  using erased_data_t = Any::erased_data_t;
  using trait_t = Any::trait_t;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using Any::Any;

  typed_any(V const& v) : Any(v) {}
  typed_any(V&& v) : Any(std::move(v)) {}
  typed_any(Any i) : Any(i) { check_type_match<V>(get_runtime(*this)); }

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
    requires !is_const
  {
    return *unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* operator->() const
    requires !is_const
  {
    return unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  value_t* get() const
    requires !is_const
  {
    return unchecked_unerase_cast<value_t>(this->erased_data_);
  }
  explicit operator bool() const {
    return static_cast<bool>(this->erased_data_);
  }
};

template <typename V, is_any Any>
bool has_data(typed_any<V, Any> const& vv) {
  return has_data(vv.erased_data_);
}
template <typename V, is_any Any>
void const* get_void_data_ptr(typed_any<V, Any> const& vv)
  requires is_const_void<typename Any::void_t>
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any Any>
void* get_void_data_ptr(typed_any<V, Any> const& vv)
  requires(!is_const_void<typename Any::void_t>)
{
  return get_void_data_ptr(vv.erased_data_);
}
template <typename V, is_any Any>
auto get_meta(typed_any<V, Any> const& vv) {
  return Any::trait_t::meta(vv.erased_data_);
}

template <typename V, is_any Any>
auto as(Any source) {
  return typed_any<V, Any>{std::move(source)};
}

template <typename To, typename From, is_any Any>
auto as(typed_any<From, Any> source)
  requires std::convertible_to<From*, To*>
{
  if constexpr (typed_any<From, Any>::is_const) {
    return typed_any<To const, Any>{std::move(source.erased_data_)};
  } else {
    return typed_any<To, Any>{std::move(source.erased_data_)};
  }
}

// --------------------------------------------------------------------------------
// any borrow, clone, lock, move

template <is_any TO_ANYPP, is_erased_data ANY_FROM>
  requires borrowable_from<typename TO_ANYPP::erased_data_t, ANY_FROM>
std::expected<TO_ANYPP, cast_error> borrow_as(ANY_FROM const& vv_from,
                                              const meta_data& meta_data) {
  using to = typename TO_ANYPP::erased_data_t;
  return query_v_table<TO_ANYPP>(meta_data).transform(
      [&](auto v_table) { return TO_ANYPP{borrow_as<to>(vv_from), v_table}; });
}

template <is_any TO_ANYPP, is_any FROM_ANYPP>
  requires borrowable_from<typename TO_ANYPP::erased_data_t,
                           typename FROM_ANYPP::erased_data_t>
auto borrow_as(FROM_ANYPP const& from_interface) {
  return borrow_as<TO_ANYPP>(get_erased_data(from_interface),
                             get_runtime(from_interface));
}

template <is_any TO_ANYPP, is_erased_data ANY_FROM>
std::expected<TO_ANYPP, cast_error> clone_to(ANY_FROM const& vv_from,
                                             const meta_data& meta_data) {
  using vv_to_t = typename TO_ANYPP::erased_data_t;
  static_assert(is_erased_data<vv_to_t>);
  return query_v_table<TO_ANYPP>(meta_data).transform([&](auto v_table) {
    return TO_ANYPP{clone_to<vv_to_t>(vv_from, meta_data), v_table};
  });
}

template <is_any TO_ANYPP, is_any FROM_ANYPP>
auto clone_to(const FROM_ANYPP& from_interface) {
  return clone_to<TO_ANYPP>(get_erased_data(from_interface),
                            get_runtime(from_interface));
}

template <is_any FROM_ANYPP>
  requires std::same_as<typename FROM_ANYPP::erased_data_t, weak>
auto lock(FROM_ANYPP const& from_interface) {
  using to_interface_t = FROM_ANYPP::template type_for<shared_const>;
  static_assert(is_any<to_interface_t>);
  using return_t = std::optional<to_interface_t>;
  if (auto shared_const = get_erased_data(from_interface).lock())
    return return_t{{shared_const, get_v_table(from_interface)}};
  return return_t{};
}

template <is_any TO_ANYPP, is_erased_data ANY_FROM>
TO_ANYPP move_to(ANY_FROM&& vv_from, const meta_data& get_meta_data) {
  using vv_to_t = typename TO_ANYPP::erased_data_t;
  static_assert(is_erased_data<vv_to_t>);
  auto v_table = query_v_table<TO_ANYPP>(get_meta_data);
  return TO_ANYPP{move_to<vv_to_t>(std::move(vv_from)), *v_table};
}

template <is_any TO_ANYPP, is_any FROM_ANYPP>
TO_ANYPP move_to(FROM_ANYPP&& from_interface) {
  return move_to<TO_ANYPP>(move_erased_data(std::move(from_interface)),
                           get_runtime(from_interface));
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

  using callee = std::function<R(typename super const&, Args...)>;

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

class unkonwn_factory_key_error : error {
  using error::error;
};

template <template <typename> typename Any, typename KEY, typename... Args>
class factory {
  using constructor_t = std::function<Any<unique>(Args...)>;
  std::map<KEY, constructor_t> function_map_;

 public:
  void register_(auto const& key, auto const& construct) {
    function_map_[key] = construct;
  }
  Any<unique> construct(auto key, Args&&... args) {
    if (auto found = function_map_.find(key); found != function_map_.end())
      return construct(std::forward<Args>(args)...);
    throw unkonwn_factory_key_error{std::to_string(key)};
  };
};

// --------------------------------------------------------------------------------
// extension member

#ifdef ANY_DLL_MODE
template <typename OBJECT_TYPE>
std::size_t& members_count();
#else
template <typename OBJECT_TYPE>
std::size_t& members_count() {
  static std::size_t count = 0;
  return count;
}
#endif

template <typename OBJECT_TYPE>
struct members {
  members() : table_(members_count<OBJECT_TYPE>()) {}
  std::vector<value> table_;
  template <typename OBJECT_MEMBER, typename ARG>
  void set(OBJECT_MEMBER member, ARG&& arg) {
    using value_t = typename OBJECT_MEMBER::value_t;
    table_[member.index] = make_value<value_t>(std::forward<ARG>(arg));
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t const* get(OBJECT_MEMBER member) const {
    const auto& value = table_[member.index];
    if (!value) return {};
    return unchecked_unerase_cast<typename OBJECT_MEMBER::value_t>(value);
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t* get(OBJECT_MEMBER member) {
    auto& value = table_[member.index];
    if (!value) return {};
    return unchecked_unerase_cast<typename OBJECT_MEMBER::value_t>(value);
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t& operator[](OBJECT_MEMBER member) {
    if (auto value = get(member)) {
      return *value;
    }
    using value_t = typename OBJECT_MEMBER::value_t;
    set(member, value_t());
    return *get(member);
  }
};

template <typename OBJECT_TYPE, typename VALUE_TYPE>
struct member {
  using object_t = OBJECT_TYPE;
  using value_t = VALUE_TYPE;
  std::size_t index = members_count<OBJECT_TYPE>()++;
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

template <typename ARG>
struct translate_erased_function_param {
  using type = ARG;
};
template <is_any Any>
struct translate_erased_function_param<virtual_<Any>> {
  using type = typename Any::void_t;
};

template <typename RET, typename... Args>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<Args>::type...);
};

template <std::size_t COUNT, typename... Args>
constexpr std::size_t dispatch_dimension_count = COUNT;
template <std::size_t COUNT, is_any Any, typename... Args>
constexpr std::size_t dispatch_dimension_count<COUNT, virtual_<Any>, Args...> =
    dispatch_dimension_count<COUNT + 1, Args...>;

template <typename R, typename... Classes>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... Args>
  struct striped_virtuals {
    static R function(Classes*... classes, Args... args) {
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

template <typename R, typename... Args>
struct dispatch_default {
  template <is_any... Anys>
  struct inner {
    template <typename... Args>
    struct implemenation {
      struct type {
        using function_t = hook<R(Anys const&..., Args...)>;
        static auto function() {
          return
              [](auto super, Anys const&..., Args... args) -> R { return R{}; };
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

  using type = outer<Args...>::template type<>;
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

  using dispatch_indices = std::array<std::size_t, dimension_count>;

  using dispatch_default_t = typename dispatch_default<R, Args...>::type;
  dispatch_default_t::function_t dispatch_default_hook_;
  dispatch_default_t::function_t::connection default_connection_ =
      dispatch_default_hook_.insert(dispatch_default_t::function());

  template <bool IsMultiDimensional, std::size_t Dimension, typename... Args>
  struct dispatch_access;

  template <std::size_t Dimension, typename... Args>
  struct dispatch_access<true, Dimension, Args...> {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename DispatchMatrix, typename ArgsTuple>
    std::optional<R> invoke(DispatchMatrix const& target,
                            ArgsTuple&& dispatch_args_tuple, auto&&...) const {
      if (!target) return {};
      return std::apply(target, std::forward<ArgsTuple>(dispatch_args_tuple));
    }
  };

  template <std::size_t Dimension, is_any Any, typename... Args>
  struct dispatch_access<true, Dimension, virtual_<Any>, Args...>
      : dispatch_access<true, Dimension + 1, Args...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    using next_t = dispatch_access<true, Dimension + 1, Args...>;

    std::size_t index_ = dispatchs_count<v_table_t>()++;
    std::size_t dispatch_dimension_size_ = 0;

    template <typename CLASS, typename... Classes>
    auto define(auto fp, auto& matrix) {
      auto dispatch_table = dispatch_table_instance<v_table_t, CLASS>();
      auto dispatch_index =
          *get_multi_dispatch_index_at(dispatch_table, index_)
               .or_else([&] -> std::optional<std::size_t> {
                 set_multi_dispatch_index_at(dispatch_table, index_,
                                             dispatch_dimension_size_);
                 return dispatch_dimension_size_++;
               });
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
      if (!dispatch_dim) return {};
      if (target.size() < *dispatch_dim + 1) return {};
      return next_t::invoke(target[*dispatch_dim],
                            std::forward<ArgsTuple>(dispatch_args_tuple),
                            std::forward<ActualArgs>(actual_args)...);
    }
  };

  template <is_any Any, typename... Args>
  struct dispatch_access<false, 0, virtual_<Any>, Args...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    std::size_t index_ = dispatchs_count<v_table_t>()++;

    template <typename CLASS>
    auto define(auto fp, auto&) {
      auto v_table = dispatch_table_instance<v_table_t, CLASS>();
      insert_function(v_table, index_, fp);
      return fp;
    }
    template <typename DispatchMatrix, typename ArgsTuple,
              typename... ActualArgs>
    std::optional<R> invoke(DispatchMatrix const&,
                            ArgsTuple&& dispatch_args_tuple, Any const& any,
                            ActualArgs&&... actual_args) const {
      auto v_table = get_v_table(any)->dispatch_table;
      auto target = get_function(v_table, index_);
      if (!target) return {};
      auto erased_function = reinterpret_cast<erased_function_t>(target);
      return std::apply(erased_function,
                        std::forward<ArgsTuple>(dispatch_args_tuple));
    }
  };

  dispatch_access<(dimension_count > 1), 0, Args...> dispatch_access_;

 public:
  template <typename... Classes>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, Classes...>::template instance<Args...>(f);
    return dispatch_access_.template define<Classes...>(fp, dispatch_matrix_);
  };
  template <typename... ActualArgs>
  auto operator()(ActualArgs&&... actual_args) const {
    auto dispatch_args_tuple = args_to_tuple<Args...>{}(
        std::tuple<>{}, std::forward<ActualArgs>(actual_args)...);
    return *dispatch_access_
                .invoke(dispatch_matrix_, dispatch_args_tuple,
                        std::forward<ActualArgs>(actual_args)...)
                .or_else([&]() -> std::optional<R> {
                  return std::invoke(dispatch_default_hook_,
                                     std::forward<ActualArgs>(actual_args)...);
                });
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

#define ANY_RUNTIME_FWD(export_, ...)                            \
  template <>                                                    \
  export_ const std::type_info& anyxx::typeid_of<__VA_ARGS__>(); \
  template <>                                                    \
  export_ anyxx::meta_data& anyxx::get_meta_data<__VA_ARGS__>();

#define ANY_RUNTIME_INSTANCE(...)                                       \
  template <>                                                           \
  const std::type_info& anyxx::typeid_of<__VA_ARGS__>() {               \
    return typeid(__VA_ARGS__);                                         \
  }                                                                     \
  template <>                                                           \
  anyxx::meta_data& anyxx::get_meta_data<std::decay_t<__VA_ARGS__>>() { \
    return runtime_implementation<__VA_ARGS__>();                       \
  }

#else

#define ANY_RUNTIME_FWD(...)
#define ANY_RUNTIME_INSTANCE(...)

#endif

#define ANY_RUNTIME_STATIC(...)  \
  ANY_RUNTIME_FWD(, __VA_ARGS__) \
  ANY_RUNTIME_INSTANCE(__VA_ARGS__)

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
#define _detail_RMCVREF(x)                                  \
  typename std::remove_const<typename std::remove_volatile< \
      typename std::remove_reference<x>::type>::type>::type
#define _detail_PARENS ()
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
#define _detail_PARAM_LIST_2H(b, c, f, ...)              \
  f c __VA_OPT__(, _detail_PARAM_LIST_2A _detail_PARENS( \
                       b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_2A() _detail_PARAM_LIST_2H
#define _detail_PARAM_LIST2(...) \
  _detail_EXPAND_(_detail_PARAM_LIST_2H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_LEAD_COMMA_H(...) __VA_OPT__(, )
#define _detail_ANYPP_FPD_H(l) _detail_ANYPP_FUNCTION_PTR_DECL l
#define _detail_ANYPP_MEMEBER_LIMP_H(l) _detail_ANYPP_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_ANYPP_FREE_LIMP_H(l) _detail_ANYPP_LAMBDA_TO_FREE_IMPL l
#define _detail_ANYPP_METHOD_H(l) _detail_ANYPP_METHOD l

#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define _add_head_1(a, ...) a, __VA_ARGS__
#define _add_head(a, x) _add_head_1(a, _strip_braces x)
#define _strip_braces _strip_braces_1
#define _strip_braces_1(...) __VA_ARGS__

#define _typename _typename1
#define _typename1(t) t

#define _detail_ANYPP_TEMPLATE_FORMAL_ARG_H(l) \
  _detail_ANYPP_TEMPLATE_FORMAL_ARG l
#define _detail_ANYPP_TEMPLATE_FORMAL_ARG(_typename) , typename _typename
#define _detail_ANYPP_TEMPLATE_FORMAL_ARGS(...) \
  __VA_OPT__(_detail_ANYPP_TEMPLATE_FORMAL_ARGS1(__VA_ARGS__))
#define _detail_ANYPP_TEMPLATE_FORMAL_ARGS1(h, ...) \
  typename _typename h __VA_OPT__(                  \
      _detail_ANYPP_TEMPLATE_FORMAL_ARGS2((__VA_ARGS__)))
#define _detail_ANYPP_TEMPLATE_FORMAL_ARGS2(l)               \
  _detail_foreach_macro(_detail_ANYPP_TEMPLATE_FORMAL_ARG_H, \
                        _detail_EXPAND_LIST l)

#define _detail_ANYPP_TEMPLATE_ARG_H(l) _detail_ANYPP_TEMPLATE_ARG l

#define _detail_ANYPP_TEMPLATE_ARG(_typename) , _typename

#define _detail_ANYPP_TEMPLATE_ARGS(...) \
  __VA_OPT__(_detail_ANYPP_TEMPLATE_ARGS1(__VA_ARGS__))

#define _detail_ANYPP_TEMPLATE_ARGS1(h, ...) \
  _typename h __VA_OPT__(_detail_ANYPP_TEMPLATE_ARGS2((__VA_ARGS__)))

#define _detail_ANYPP_TEMPLATE_ARGS2(l) \
  _detail_foreach_macro(_detail_ANYPP_TEMPLATE_ARG_H, _detail_EXPAND_LIST l)

#define _detail_ANYPP_V_TABLE_TEMPLATE_HEADER_H(...) \
  __VA_OPT__(template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_ANYPP_V_TABLE_TEMPLATE_HEADER(t) \
  _detail_ANYPP_V_TABLE_TEMPLATE_HEADER_H t

#define _detail_ANYPP_V_TABLE_TEMPLATE_INSTANCE_H(...) \
  __VA_OPT__(<_detail_ANYPP_TEMPLATE_ARGS(__VA_ARGS__)>)

#define _detail_ANYPP_V_TABLE_TEMPLATE_INSTANCE(t) \
  _detail_ANYPP_V_TABLE_TEMPLATE_INSTANCE_H t

#define _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS_H(...) \
  __VA_OPT__(<_detail_ANYPP_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS(t) \
  _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS_H t

#define _detail_TA_H(l) _detail_TA l

#define _detail_TA(type) , type

#define _detail_ANYPP_MAP_LIMP_H(l) _detail_ANYPP_MAP_IMPL l

#define _detail_ANYPP_MAP_IMPL(type, name, name_ext, exact_const, const_, ...) \
  auto name(T const_* x __VA_OPT__(                                            \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {                  \
    return (*x).name_ext(                                                      \
        __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));                 \
  };

#define _detail_ANYPP_FUNCTION_PTR_DECL(type, name, name_ext, exact_const, \
                                        const_, ...)                       \
  type (*name)(void const_* __VA_OPT__(, __VA_ARGS__));

#define _detail_ANYPP_LAMBDA_TO_MEMEBER_IMPL(type, name, name_ext,     \
                                             exact_const, const_, ...) \
  name = [](void const_* _vp __VA_OPT__(                               \
             , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {   \
    return v_table_map{}.name(                                         \
        anyxx::unchecked_unerase_cast<Concrete>(_vp) __VA_OPT__(, )    \
            __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));     \
  };

#define _detail_ANYPP_METHOD(type, name, name_ext, exact_const, const_, ...) \
  type name_ext(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const \
    requires(::anyxx::const_correct_call_for_erased_data<                    \
             void const_*, erased_data_t, exact_const>)                      \
  {                                                                          \
    return static_cast<v_table_t*>(v_table_)->name(                          \
        anyxx::get_void_data_ptr(base_t::erased_data_)                       \
            __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));         \
  }

#define _detail_ANYPP_MAP_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_MAP_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYPP_V_TABLE_FUNCTION_PTRS(...)        \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_FPD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYPP_V_TABLE_LAMBDAS(...)                       \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_MEMEBER_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYPP_METHODS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_METHOD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define ANY_TEMPLATE_(t, n, BASE, l)                                           \
                                                                               \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(_add_head((ErasedData), t))>    \
  struct n;                                                                    \
                                                                               \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>             \
  struct n##_default_v_table_map {                                             \
    _detail_ANYPP_MAP_FUNCTIONS(l)                                             \
  };                                                                           \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>             \
  struct n##_v_table_map                                                       \
      : n##_default_v_table_map<_detail_ANYPP_TEMPLATE_FORMAL_ARGS(            \
            _add_head((T), t))> {};                                            \
                                                                               \
  _detail_ANYPP_V_TABLE_TEMPLATE_HEADER(t) struct n##_v_table;                 \
                                                                               \
  _detail_ANYPP_V_TABLE_TEMPLATE_HEADER(t) struct n##_v_table                  \
      : BASE##_v_table,                                                        \
        anyxx::dispatch_holder<anyxx::has_dispatchs<n>, n> {                   \
    using v_table_base_t = BASE##_v_table;                                     \
    using v_table_t = n##_v_table;                                             \
    using own_dispatch_holder_t =                                              \
        anyxx::dispatch_holder<anyxx::has_dispatchs<n>, n>;                    \
                                                                               \
    static bool static_is_derived_from(const std::type_info& from) {           \
      return typeid(v_table_t) == from                                         \
                 ? true                                                        \
                 : v_table_base_t::static_is_derived_from(from);               \
    }                                                                          \
                                                                               \
    _detail_ANYPP_V_TABLE_FUNCTION_PTRS(l);                                    \
                                                                               \
    static constexpr bool dispatchs_enabled = anyxx::has_dispatchs<n>;         \
                                                                               \
    template <typename Concrete>                                               \
    n##_v_table(std::in_place_type_t<Concrete> concrete)                       \
        : v_table_base_t(concrete) {                                           \
      using v_table_map = n##_v_table_map<_detail_ANYPP_TEMPLATE_ARGS(         \
          _add_head((Concrete), t))>;                                          \
                                                                               \
      _detail_ANYPP_V_TABLE_LAMBDAS(l);                                        \
                                                                               \
      if constexpr (dispatchs_enabled) {                                       \
        own_dispatch_holder_t::dispatch_table =                                \
            ::anyxx::dispatch_table_instance<n##_v_table, Concrete>();         \
      }                                                                        \
                                                                               \
      ::anyxx::set_is_derived_from<v_table_t>(this);                           \
    };                                                                         \
                                                                               \
    template <typename Concrete>                                               \
    static auto imlpementation() {                                             \
      return anyxx::v_table_instance_implementaion<v_table_t, Concrete>();     \
    }                                                                          \
  };                                                                           \
                                                                               \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(_add_head((ErasedData), t))>    \
  struct n : BASE<ErasedData> {                                                \
    using erased_data_t = ErasedData;                                          \
    using base_t = BASE<ErasedData>;                                           \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t =                                                          \
        n##_v_table _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS(t);             \
                                                                               \
    template <typename Concrete>                                               \
    static auto v_table_imlpementation() {                                     \
      static_assert(!anyxx::is_any<Concrete>);                                 \
      return v_table_t::template imlpementation<Concrete>();                   \
    }                                                                          \
                                                                               \
    using base_t::erased_data_;                                                \
    using base_t::v_table_;                                                    \
                                                                               \
    n(erased_data_t erased_data, v_table_t* v_table)                           \
        : base_t(std::move(erased_data), v_table) {}                           \
    template <typename ConstructedWith>                                        \
    n(ConstructedWith&& v)                                                     \
      requires anyxx::constructibile_for<ConstructedWith, ErasedData>          \
        : base_t(std::forward<ConstructedWith>(v)) {                           \
      v_table_ = v_table_t::template imlpementation<                           \
          anyxx::unerased<ErasedData, ConstructedWith>>();                     \
    }                                                                          \
    template <typename Otther>                                                 \
    n(const Otther& other)                                                     \
      requires(std::derived_from<typename Otther::v_table_t, v_table_t> &&     \
               anyxx::borrowable_from<erased_data_t,                           \
                                      typename Otther::erased_data_t>)         \
        : base_t(other) {}                                                     \
    template <anyxx::is_any Otther>                                            \
    n(Otther&& other) noexcept                                                 \
      requires(                                                                \
          std::derived_from<Otther::v_table_t, v_table_t> &&                   \
          anyxx::moveable_from<erased_data_t, typename Otther::erased_data_t>) \
        : base_t(std::move(other)) {}                                          \
                                                                               \
    _detail_ANYPP_METHODS(l)                                                   \
                                                                               \
        auto const*                                                            \
        operator->() const {                                                   \
      return this;                                                             \
    }                                                                          \
                                                                               \
    using base_t::operator();                                                  \
    using base_t::operator[];                                                  \
                                                                               \
    n() = default;                                                             \
    n(n const&) = default;                                                     \
    n(n&&) = default;                                                          \
    n& operator=(n const&) = default;                                          \
    n& operator=(n&&) = default;                                               \
    template <anyxx::is_erased_data Otther>                                    \
    friend class anyxx::any_base;                                              \
    template <anyxx::is_any To, anyxx::is_any From>                            \
    friend To anyxx::unchecked_downcast_to(From from)                          \
      requires(std::derived_from<To, From>);                                   \
    template <anyxx::is_erased_data Otther>                                    \
    using type_for = n<_detail_ANYPP_TEMPLATE_ARGS(_add_head((Otther), t))>;   \
  };

//    n(n&) = default;                                                           \

#define ANY_(n, BASE, l) ANY_TEMPLATE_((), n, BASE, l)

#define ANY(n, ...) ANY_(n, ::anyxx::any_base, __VA_ARGS__)

#define ANY_TEMPLATE(t, n, l) ANY_TEMPLATE_(t, n, ::anyxx::any_base, l)

#define ANY_METHOD_(...) (__VA_ARGS__)

#define ANY_METHOD(ret, name, ...) \
  ANY_METHOD_(ret, name, name, false, , __VA_ARGS__)

#define ANY_CONST_METHOD(ret, name, ...) \
  ANY_METHOD_(ret, name, name, false, const, __VA_ARGS__)

#define ANY_OP(ret, op, ...)                                                  \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, false, , \
              __VA_ARGS__)

#define ANY_OP_EXACT(ret, op, ...)                                           \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, true, , \
              __VA_ARGS__)

#define ANY_CONST_OP(ret, op, ...)                                          \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, false, \
              const, __VA_ARGS__)

#define ANY_CONST_OP_EXACT(ret, op, ...)                                   \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, true, \
              const, __VA_ARGS__)

#define ANY_FORWARD(interface_namespace, interface_name, ...) \
  namespace interface_namespace {                             \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(               \
      _add_head((ErasedData), (__VA_ARGS__)))>                \
  struct interface_name;                                      \
  struct interface_name##_v_table;                            \
  }

#define ANY_REGISTER_V_TABLE_INSTANCE(class_, interface_)               \
  namespace {                                                           \
  static auto __ =                                                      \
      anyxx::bind_v_table_to_meta_data<interface_##_v_table, class_>(); \
  }

#ifdef ANY_DLL_MODE

#define ANY_V_TABLE_INSTANCE_FWD(export_, class_, interface_namespace_,      \
                                 interface_)                                 \
  ANY_FORWARD(interface_namespace_, interface_)                              \
  namespace anyxx {                                                          \
  template <>                                                                \
  export_ interface_namespace_::interface_##_v_table*                        \
  v_table_instance_implementaion<interface_namespace_::interface_##_v_table, \
                                 class_>();                                  \
  }

#define ANY_V_TABLE_INSTANCE(class_, interface_namespace_, interface_) \
  template <>                                                          \
  interface_namespace_::interface_##_v_table*                          \
  anyxx::v_table_instance_implementaion<                               \
      interface_namespace_::interface_##_v_table, class_>() {          \
    static interface_namespace_::interface_##_v_table v_table{         \
        std::in_place_type<class_>};                                   \
    return &v_table;                                                   \
  }                                                                    \
  ANY_REGISTER_V_TABLE_INSTANCE(class_, interface_namespace_::interface_)

// TODO
// #define ANY_V_TABLE_TEMPLATE_INSTANCE(export_, class, interface_, ...) \
//  template <>                                                         \
//  struct export_ interface_##_v_table_instance<class, __VA_ARGS__> {  \
//    static interface_##_v_table<__VA_ARGS__>* get() {                 \
//      static interface_##_v_table<__VA_ARGS__> v_table{               \
//          std::in_place_type<class>};                                 \
//      return &v_table;                                                \
//    };                                                                \
//  };                                                                  \
//  namespace {                                                         \
//  static auto __ = anyxx::bind_v_table_to_meta_data<           \
//      interface_##_v_table_instance<class, __VA_ARGS__>, class>();    \
//  }

#else

#define ANY_V_TABLE_INSTANCE_FWD(...)
#define ANY_V_TABLE_INSTANCE(...)

#endif

#define ANY_V_TABLE_INSTANCE_STATIC(class_, interface_, interface_namespace_) \
  ANY_V_TABLE_INSTANCE_FWD(, class_, interface_, interface_namespace_)        \
  ANY_V_TABLE_INSTANCE(, class_, interface_, interface_namespace_)

#define ANY_HAS_DISPATCH(interface_namespace, interface_name, ...)          \
  ANY_FORWARD(interface_namespace, interface_name, __VA_ARGS__)             \
                                                                            \
  namespace anyxx {                                                         \
  template <>                                                               \
  constexpr bool has_dispatchs<interface_namespace::interface_name> = true; \
  }

#define ANY_NAME(...) __VA_ARGS__
