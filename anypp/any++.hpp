#pragma once

#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <expected>
#include <map>
#include <optional>
#include <ranges>
#include <typeindex>
#include <variant>
#include <vector>
#include <utility>

namespace anypp 
{

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

template <typename DATA>
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

template <is_erased_data DATA>
using data_void = trait<DATA>::void_t;

template <typename ERASED_DATA>
concept is_const_data =
    is_erased_data<ERASED_DATA> && is_const_void<data_void<ERASED_DATA>>;

template <typename ERASED_DATA>
concept is_weak_data =
    is_erased_data<ERASED_DATA> && trait<ERASED_DATA>::is_weak;

template <bool CALL_IS_CONST, bool ERASED_DATA_IS_CONST,
          bool ERASED_DATA_IS_WEAK>
concept const_correct_call =
    !ERASED_DATA_IS_WEAK &&
    ((CALL_IS_CONST == ERASED_DATA_IS_CONST) || !ERASED_DATA_IS_CONST);

template <typename CALL, typename ERASED_DATA, bool EXACT>
concept const_correct_call_for_erased_data =
    !is_weak_data<ERASED_DATA> && voidness<CALL> &&
    is_erased_data<ERASED_DATA> &&
    ((EXACT && (is_const_void<CALL> == is_const_data<ERASED_DATA>)) ||
     (!EXACT &&
      const_correct_call<is_const_void<CALL>, is_const_data<ERASED_DATA>,
                         is_weak_data<ERASED_DATA>>));

template <is_erased_data ERASED_DATA, typename FROM>
ERASED_DATA erased(FROM&& from) {
  return trait<ERASED_DATA>::erase(std::forward<FROM>(from));
}

template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH>
using unerased =
    trait<ERASED_DATA>::template unerased<std::decay_t<CONSTRUCTED_WITH>>;

template <is_erased_data ERASED_DATA>
bool has_data(ERASED_DATA const& vv) {
  return trait<ERASED_DATA>::has_value(vv);
}
template <is_erased_data ERASED_DATA>
void const* get_void_data_ptr(ERASED_DATA const& vv)
  requires std::same_as<void const*, typename trait<ERASED_DATA>::void_t>
{
  return trait<ERASED_DATA>::value(vv);
}
template <is_erased_data ERASED_DATA>
void* get_void_data_ptr(ERASED_DATA const& vv)
  requires std::same_as<void*, typename trait<ERASED_DATA>::void_t>
{
  return trait<ERASED_DATA>::value(vv);
}

template <typename U>
auto unchecked_unerase_cast(void const* p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(void* p) {
  return static_cast<U*>(p);
}

template <typename U, is_erased_data ERASED_DATA>
auto unchecked_unerase_cast(ERASED_DATA const& o) {
  return unchecked_unerase_cast<U>(get_void_data_ptr(o));
}
template <typename U, is_erased_data ERASED_DATA>
auto unchecked_unerase_cast(ERASED_DATA const& o)
  requires(!is_const_data<ERASED_DATA>)
{
  return unchecked_unerase_cast<U>(get_void_data_ptr(o));
}

template <typename U, is_erased_data ERASED_DATA>
auto unerase_cast(ERASED_DATA const& o, meta_data const& meta) {
  check_type_match<U>(meta);
  return unchecked_unerase_cast<U>(o);
}
template <typename U, is_erased_data ERASED_DATA>
U const* unerase_cast(ERASED_DATA const* o, meta_data const& meta) {
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}
template <typename U, is_erased_data ERASED_DATA>
U* unerase_cast(ERASED_DATA const* o, meta_data const& meta)
  requires(!is_const_data<ERASED_DATA>)
{
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}

template <typename CONSTRUCTED_WITH, typename ERASED_DATA, typename BASE>
concept erased_constructibile_for =
    !std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, BASE> &&
    !is_erased_data<std::remove_cvref_t<CONSTRUCTED_WITH>> &&
    (!std::is_const_v<std::remove_reference_t<CONSTRUCTED_WITH>> ||
     trait<ERASED_DATA>::is_constructibile_from_const);

// --------------------------------------------------------------------------------
// erased data observer

template <voidness VOIDNESS>
using observer = VOIDNESS;
using const_observer = observer<const_void>;
using mutable_observer = observer<mutable_void>;

template <typename ERASED_DATA, voidness VOIDNESS>
struct observer_trait {
  using void_t = VOIDNESS;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;
  static constexpr bool is_owner = false;
  static constexpr bool is_weak = false;

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  static VOIDNESS value(const auto& ptr) { return ptr; }

  template <typename CONSTRUCTED_WITH>
  using unerased = CONSTRUCTED_WITH;

  template <typename V>
  static auto erase(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return ERASED_DATA(static_cast<VOIDNESS>(&v));
  }
  template <typename V>
  static auto erase(const V& v)
    requires(is_const)
  {
    return ERASED_DATA(static_cast<VOIDNESS>(&v));
  }
};

template <>
struct trait<const_observer>
    : observer_trait<const_observer, const_observer> {};
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

  template <typename CONSTRUCTED_WITH>
  using unerased = std::decay_t<typename CONSTRUCTED_WITH::element_type>;

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

  template <typename CONSTRUCTED_WITH>
  using unerased = std::decay_t<typename CONSTRUCTED_WITH::element_type>;

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

template <typename T, typename... ARGS>
auto make_unique(ARGS&&... args) {
  return move_to_unique(std::make_unique<T>(std::forward<ARGS>(args)...));
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

  template <typename CONSTRUCTED_WITH>
  using unerased = typename CONSTRUCTED_WITH::element_type;

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
  template <typename CONCRETE>
  any_base_v_table(std::in_place_type_t<CONCRETE> concrete);

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

using extension_method_table_function_t = void (*)();
using extension_method_table_dispatch_index_t = std::size_t;
using extension_method_table_entry_t =
    std::variant<std::monostate, extension_method_table_dispatch_index_t,
                 extension_method_table_function_t>;
using extension_method_table_t = std::vector<extension_method_table_entry_t>;

void insert_function(extension_method_table_t* v_table, std::size_t index,
                     auto fp) {
  if (v_table->size() <= index) v_table->resize(index + 1);
  v_table->at(index) =
      reinterpret_cast<extension_method_table_function_t>(fp);
}
inline extension_method_table_function_t get_function(
    extension_method_table_t* v_table, std::size_t index) {
  if (v_table->size() <= index) return {};
  if (auto f =
          std::get_if<extension_method_table_function_t>(&v_table->at(index)))
    return *f;
  else
    return {};
}

inline std::optional<std::size_t> get_multi_method_index_at(
    extension_method_table_t* v_table, std::size_t index) {
  if (v_table->size() <= index) return {};
  if (auto extension_method_table_dispatch_index =
          std::get_if<extension_method_table_dispatch_index_t>(
              &v_table->at(index)))
    return *extension_method_table_dispatch_index;
  return {};
}
inline void set_multi_method_index_at(
    extension_method_table_t* v_table, std::size_t index_multi_method,
    extension_method_table_dispatch_index_t
        dispatch_index_of_class_in_dispatch_matrix) {
  if (v_table->size() <= index_multi_method)
    v_table->resize(index_multi_method + 1);
  v_table->at(index_multi_method) = dispatch_index_of_class_in_dispatch_matrix;
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

template <typename CONCRETE>
any_base_v_table::any_base_v_table(std::in_place_type_t<CONCRETE> concrete)
    : _is_derived_from([](const std::type_info& from) {
        return static_is_derived_from(from);
      }) {}

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance_implementation() {
  static extension_method_table_t extension_method_table;
  return &extension_method_table;
}

#ifdef ANY_DLL_MODE

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance();

#else

template <typename EXTENDED_V_TABLE, typename CLASS_NAME>
extension_method_table_t* extension_method_table_instance() {
  return extension_method_table_instance_implementation<EXTENDED_V_TABLE,
                                                        CLASS_NAME>();
}

#endif  //

template <typename V_TABLE, typename CONCRETE>
auto bind_v_table_to_meta_data() {
  auto v_table = V_TABLE::template imlpementation<CONCRETE>();
  get_meta_data<CONCRETE>().register_v_table(v_table);
  return v_table;
}

template <typename U>
bool type_match(meta_data const& meta_data) {
  return &meta_data.get_type_info() == &typeid_of<std::decay_t<U>>();
}

// --------------------------------------------------------------------------------
// borrow erased data

template <is_erased_data TO, is_erased_data FROM>
struct borrow_trait;

template <typename TO, typename FROM>
concept borrowable_from =
    is_erased_data<FROM> && is_erased_data<TO> && requires(FROM f) {
      { borrow_trait<TO, FROM>{}(f) } -> std::same_as<TO>;
    };

template <is_erased_data FROM>
  requires(!is_const_data<FROM> && !is_weak_data<FROM>)
struct borrow_trait<mutable_observer, FROM> {
  auto operator()(const auto& from) {
    return mutable_observer{get_void_data_ptr(from)};
  }
};
template <is_erased_data FROM>
  requires(!is_weak_data<FROM>)
struct borrow_trait<const_observer, FROM> {
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

template <typename TO, typename FROM>
  requires borrowable_from<TO, FROM>
TO borrow_as(FROM const& from) {
  return borrow_trait<TO, FROM>{}(from);
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

template <typename TO>
concept cloneable_to =
    is_erased_data<TO> && trait<TO>::is_owner;

template <is_erased_data TO, is_erased_data FROM>
  requires cloneable_to<TO>
TO clone_to(FROM const& from, auto const& meta_data) {
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

template <is_erased_data TO, is_erased_data FROM>
bool constexpr can_move_to_from = false;

template <typename TO, typename FROM>
concept moveable_from =
    is_erased_data<FROM> && is_erased_data<TO> && can_move_to_from<TO, FROM>;

template <is_erased_data X>
bool constexpr can_move_to_from<X, X> = true;

template <>
bool constexpr can_move_to_from<shared_const, unique> = true;

template <>
bool constexpr can_move_to_from<weak, shared_const> = true;

template <voidness TO, voidness FROM>
  requires const_correct_call<is_const_void<TO>, is_const_void<FROM>, is_weak_data<FROM>>
bool constexpr can_move_to_from<TO, FROM> = true;

template <typename TO, typename FROM>
  requires moveable_from<TO, std::decay_t<FROM>>
TO move_to(FROM&& from) {
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

template <is_erased_data ERASED_DATA>
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

template <typename CONSTRUCTED_WITH, typename ERASED_DATA>
concept constructibile_for =
    erased_constructibile_for<CONSTRUCTED_WITH, ERASED_DATA,
                                    any_base<ERASED_DATA>> &&
    !is_any<CONSTRUCTED_WITH> &&
    !is_typed_any<std::remove_cvref_t<CONSTRUCTED_WITH>>;

template <is_erased_data ERASED_DATA>
class any_base {
 public:
  using erased_data_t = ERASED_DATA;
  using trait_t = trait<erased_data_t>;
  using void_t = typename trait_t::void_t;
  using v_table_t = any_base_v_table;

 protected:
  erased_data_t erased_data_ = {};
  v_table_t* v_table_ = nullptr;

  any_base() = default;
  any_base(erased_data_t erased_data, v_table_t* v_table)
      : erased_data_(std::move(erased_data)), v_table_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  any_base(CONSTRUCTED_WITH&& constructed_with)
    requires constructibile_for<CONSTRUCTED_WITH, ERASED_DATA>
      : erased_data_(erased<erased_data_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    using t = unerased<ERASED_DATA, CONSTRUCTED_WITH>;
  }

 public:
  template <is_any OTHER>
  any_base(const OTHER& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t> &&
             borrowable_from<erased_data_t,
                                   typename OTHER::erased_data_t>)
      : erased_data_(borrow_as<ERASED_DATA>(other.erased_data_)),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  any_base(OTHER&& other)
    requires(std::derived_from<typename OTHER::v_table_t, v_table_t> &&
             moveable_from<erased_data_t, typename OTHER::erased_data_t>)
      : erased_data_(move_to<ERASED_DATA>(std::move(other.erased_data_))),
        v_table_(get_v_table(other)) {}
  template <typename OTHER>
  any_base& operator=(OTHER&& other)
    requires(std::derived_from<OTHER::v_table_t, v_table_t>)
  {
    erased_data_ = move_to<ERASED_DATA>(std::move(other.erased_data_));
    v_table_ = get_v_table(other);
    return *this;
  }
  any_base(const any_base&) = default;
  // any_base(any_base&) requires(std::is_copy_constructible_v<any_base>) = default;
  any_base(any_base&& rhs) noexcept
      : erased_data_(std::move(rhs.erased_data_)), v_table_(rhs.v_table_) {}
  any_base& operator=(any_base const& other) = default;

  template <is_erased_data OTHER>
  friend class any_base;

  template <is_erased_data ERASED_DATA>
  friend inline auto& get_erased_data(any_base<ERASED_DATA> const& any);
  template <is_erased_data ERASED_DATA>
  friend inline auto move_erased_data(any_base<ERASED_DATA>&& any);
  template <is_erased_data ERASED_DATA>
  friend inline auto get_void_data_ptr(any_base<ERASED_DATA> const& any);
  template <is_any I>
  friend inline auto get_v_table(I const& any);

  template <is_any TO, is_any FROM>
  friend inline TO unchecked_downcast_to(FROM from)
    requires(std::derived_from<TO, FROM>);

 public:
  void operator()() const {}
  void* operator[](void*) const { return {}; }
};

template <typename V_TABLE, is_erased_data ERASED_DATA>
struct interface_t;

template <typename V_TABLE, is_erased_data ERASED_DATA>
using interface_for = typename interface_t<V_TABLE, ERASED_DATA>::type;

template <is_erased_data ERASED_DATA>
auto& get_erased_data(any_base<ERASED_DATA> const& any) {
  return any.erased_data_;
}
template <is_erased_data ERASED_DATA>
auto move_erased_data(any_base<ERASED_DATA>&& any) {
  return std::move(any.erased_data_);
}
template <is_erased_data ERASED_DATA>
auto get_void_data_ptr(any_base<ERASED_DATA> const& any) {
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
template <is_any FROM, is_erased_data VV>
bool is_derived_from(any_base<VV> const& any) {
  return is_derived_from(typeid(FROM::v_table_t), any);
}

template <typename V_TABLE>
void set_is_derived_from(auto v_table) {
  v_table->_is_derived_from = +[](const std::type_info& from) {
    return V_TABLE::static_is_derived_from(from);
  };
}

template <typename TO>
auto unchecked_v_table_downcast_to(any_base_v_table* v_table) {
  return static_cast<TO*>(v_table);
}
template <is_any TO>
auto unchecked_v_table_downcast_to(any_base_v_table* v_table) {
  return unchecked_v_table_downcast_to<typename TO::v_table_t>(v_table);
}

template <is_any Any>
inline auto get_v_table(Any const& any) {
  return unchecked_v_table_downcast_to<Any>(any.v_table_);
}

template <is_any TO, is_any FROM>
TO unchecked_downcast_to(FROM from)
  requires(std::derived_from<TO, FROM>)
{
  return TO{std::move(from.erased_data_),
            unchecked_v_table_downcast_to<TO>(from.v_table_)};
}

template <is_any TO, is_any FROM>
std::optional<TO> downcast_to(FROM from)
  requires(std::derived_from<TO, FROM>)
{
  if (is_derived_from<TO>(from))
    return {unchecked_downcast_to<TO>(std::move(from))};
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
template <typename V_TABLE, typename CONCRETE>
V_TABLE* v_table_instance_implementaion();
#else
template <typename V_TABLE, typename CONCRETE>
V_TABLE* v_table_instance_implementaion() {
  static V_TABLE v_table{std::in_place_type<CONCRETE>};
  static auto __ =
      anypp::get_meta_data<CONCRETE>().register_v_table(
          &v_table);
  return &v_table;
}
#endif  // DEBUG

template <template <typename...> typename Any>
constexpr bool has_extension_methods = false;

template <typename I>
concept has_extension_methods_enabled =
    is_any<I> && I::v_table_t::extension_methods_enabled;

template <bool HAS_EXTENSION_METHODS, template <typename...> typename Any>
struct extension_method_holder;
template <template <typename...> typename Any>
struct extension_method_holder<false, Any> {};
template <template <typename...> typename Any>
struct extension_method_holder<true, Any> {
  extension_method_table_t* extension_method_table = nullptr;
};

template <is_any TO_ANYPP>
auto query_v_table(const meta_data& meta_data)
    -> std::expected<typename TO_ANYPP::v_table_t*,
                     anypp::cast_error> {
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
  typed_any(Any i) : Any(i) {
    check_type_match<V>(get_runtime(*this));
  }

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

template <typename TO, typename FROM, is_any Any>
auto as(typed_any<FROM, Any> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (typed_any<FROM, Any>::is_const) {
    return typed_any<TO const, Any>{std::move(source.erased_data_)};
  } else {
    return typed_any<TO, Any>{std::move(source.erased_data_)};
  }
}

// --------------------------------------------------------------------------------
// query v_table



//./anypp/anypp/borrow.hpp
//./anypp/anypp/clone.hpp
//./anypp/anypp/lock.hpp
//./anypp/anypp/move.hpp

//./anypp/anypp/hook.hpp
//./anypp/anypp/factory.hpp
//./anypp/anypp/extension_member.hpp
//./anypp/anypp/extension_method.hpp

//./anypp/anypp/utillities\unnamed__.hpp
//./anypp/anypp/any_meta_class.hpp

}

#ifdef ANY_DLL_MODE

#define ANY_RUNTIME_FWD(export_, ...)               \
  template <>                                      \
  export_ const std::type_info&                    \
  anypp::typeid_of<__VA_ARGS__>(); \
  template <>                                      \
  export_ anypp::meta_data&        \
  anypp::get_meta_data<__VA_ARGS__>();

#define ANY_RUNTIME_INSTANCE(...)                                          \
  template <>                                                             \
  const std::type_info& anypp::typeid_of<__VA_ARGS__>() { \
    return typeid(__VA_ARGS__);                                           \
  }                                                                       \
  template <>                                                             \
  anypp::meta_data&                                       \
  anypp::get_meta_data<std::decay_t<__VA_ARGS__>>() {     \
    return runtime_implementation<__VA_ARGS__>();                         \
  }

#else

#define ANY_RUNTIME_FWD(...)
#define ANY_RUNTIME_INSTANCE(...)

#endif

#define ANY_RUNTIME_STATIC(...)  \
  ANY_RUNTIME_FWD(, __VA_ARGS__) \
  ANY_RUNTIME_INSTANCE(__VA_ARGS__)\
