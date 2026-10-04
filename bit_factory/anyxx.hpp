#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/infra/config.hpp>
#include <bit_factory/v23/preprocessor_meta_class.hpp>

namespace anyxx {

template <typename, typename = void>
struct is_type_complete_impl : std::false_type {};
template <typename T>
struct is_type_complete_impl<
    T, std::enable_if_t<std::is_object<T>::value &&
                        !std::is_pointer<T>::value && (sizeof(T) > 0)>>
    : std::true_type {};
template <typename T>
constexpr static inline bool is_type_complete = is_type_complete_impl<T>::value;

template <typename T, typename Rep>
using default_rep = std::conditional_t<is_type_complete<Rep>, Rep, T>;

template <class... Ts>
struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename T>
struct is_variant_impl : std::false_type {};
template <typename... Args>
struct is_variant_impl<std::variant<Args...>> : std::true_type {};
template <typename T>
inline constexpr bool is_variant = is_variant_impl<T>::value;

inline constexpr bool ignore_mapf_concept_with_always_false(bool) {
  return false;
}
inline constexpr bool ignore_mapf_concept_with_always_true(bool) {
  return true;
}
inline constexpr bool use_mapf_concept(bool b) { return b; }

inline void dummy() {}

#ifdef ANY_DLL_MODE
constexpr bool is_in_dll_mode = true;
#else
constexpr bool is_in_dll_mode = false;
#endif

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class type_mismatch_error : public error {
  using error::error;
};

template <typename Value>
struct using_;
template <typename Value>
struct using_cref;
template <typename Type>
struct trait_class;

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

constexpr std::size_t small_object_size = 4 * sizeof(mutable_void);

template <typename T>
constexpr std::size_t compute_val_proxy_size(std::size_t default_size) {
  if constexpr (anyxx::is_type_complete<T>) {
    return T::value;
  } else {
    return default_size;
  };
}

using allocate_t = mutable_void (*)();
using copy_constructor_t = mutable_void (*)(mutable_void, const_void);
template <typename VTable>
concept is_copy_constructor_v_table =
    requires(VTable* v_table, mutable_void dummy, mutable_void placement,
             const_void from) {
      { v_table->copy_constructor } -> std::convertible_to<copy_constructor_t>;
    };
using move_constructor_t = mutable_void (*)(mutable_void, mutable_void);
template <typename VTable>
concept is_move_constructor_v_table =
    requires(VTable* v_table, mutable_void placement, mutable_void from) {
      { v_table->move_constructor } -> std::convertible_to<move_constructor_t>;
    };
using destructor_t = void (*)(mutable_void);
template <typename VTable>
concept is_destructor_v_table =
    requires(VTable* v_table, mutable_void dummy, mutable_void placement) {
      { v_table->destructor } -> std::convertible_to<destructor_t>;
    };
using delete_t = void (*)(mutable_void);
template <typename VTable>
concept is_delete_v_table =
    requires(VTable* v_table, mutable_void dummy, mutable_void placement) {
      { v_table->delete_ } -> std::convertible_to<delete_t>;
    };

struct model_size_t {
  std::size_t size;
  bool trivial;
};
template <typename VTable>
concept is_model_size_v_table = requires(VTable* v_table) {
  { v_table->model_size } -> std::convertible_to<model_size_t>;
};

template <typename VTable>
concept is_type_info_v_table = requires(VTable* v_table) {
  { v_table->type_info_ } -> std::convertible_to<std::type_info const*>;
};

using is_derived_from_t = bool (*)(const std::type_info&);
template <typename VTable>
concept is_dynamic_castable_v_table = requires(VTable* v_table) {
  { v_table->is_derived_from_ } -> std::convertible_to<is_derived_from_t>;
};

using is_derived_from_t = bool (*)(const std::type_info&);
template <typename VTable>
concept is_meta_data_v_table = requires(VTable* v_table) {
  { v_table->meta_data_ } -> std::convertible_to<meta_data*>;
};

template <typename M, typename V>
concept is_op_pre_increment_model_map =
    requires(M model_map, V v) { model_map.op_pre_increment(v); };

template <typename VTable>
concept is_op_pre_increment_v_table =
    requires(VTable* v_table, mutable_void x) { v_table->op_pre_increment(x); };

/// \brief Use this type to indicate, that the ANY_TYPE must be specified in
/// the model map.
struct undefined {};

template <typename VTable, typename Concrete>
VTable* v_table_instance();

template <voidness Voidness>
using observer = Voidness;
/// Proxy to capture the dispatch target type erased by const reference
/// An any with such a proxy is lifetime bound to the object referenced!
/// \ingroup proxies
using cref = observer<const_void>;
/// Proxy to capture the dispatch target type erased by mutable reference
/// An any with such a proxy is lifetime bound to the object referenced!
/// \ingroup proxies
using mutref = observer<mutable_void>;

/// No lifetime functionality
/// Base of v-tables for traits without lifetime requirements
struct observeable_v_table {
  using v_table_t = observeable_v_table;

  struct deduced_type {};

  using val_nullable = std::false_type;
  static constexpr std::size_t val_proxy_size = small_object_size;

  /// Type-erasing constructor
  template <typename Concrete>
  explicit observeable_v_table(
      [[maybe_unused]] std::in_place_type_t<Concrete> concrete) {}

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(observeable_v_table) == from;
  }
};

template <typename T = std::nullptr_t>
struct no_model_map {
  using rep_type = T;
};

struct observeable {
  template <typename>
  static constexpr bool modeled_by() {
    return true;
  }

  using default_proxy_t = cref;

  template <typename StaticDispatchType>
  using static_dispatch_map_t = no_model_map<StaticDispatchType>;

  using v_table_t = observeable_v_table;
  using val_nullable = typename v_table_t::val_nullable;
  static constexpr std::size_t val_proxy_size = v_table_t::val_proxy_size;
};

template <typename VTable>
void set_is_derived_from(auto v_table) {
  if constexpr (anyxx::is_dynamic_castable_v_table<VTable>) {
    v_table->is_derived_from_ = +[](const std::type_info& from) {
      return VTable::static_is_derived_from(from);
    };
  }
}

inline bool is_derived_from(const std::type_info& from,
                            is_dynamic_castable_v_table auto* v_table) {
  return v_table->is_derived_from_(from);
}

inline mutable_void allocate(is_model_size_v_table auto* v_table) {
  return ::operator new(v_table->model_size.size);
}
template <typename T>
inline void deallocate(T v_table, mutable_void data) {
  if (!data) return;
  if constexpr (!std::same_as<T, std::nullptr_t>) {
    assert(v_table);
    ::operator delete(data, v_table->model_size.size);
  }
}

inline model_size_t model_size(std::nullptr_t) { return {0, true}; }
inline model_size_t model_size(is_model_size_v_table auto* v_table) {
  return v_table ? v_table->model_size : model_size_t{0, true};
}
inline mutable_void copy_construct_at(is_copy_constructor_v_table auto* v_table,
                                      mutable_void placement, const_void from) {
  return v_table->copy_constructor(placement, from);
}
inline mutable_void copy_construct(is_copy_constructor_v_table auto* v_table,
                                   const_void from) {
  return copy_construct_at(v_table, allocate(v_table), from);
}
inline mutable_void move_construct_at(is_move_constructor_v_table auto* v_table,
                                      mutable_void placement,
                                      mutable_void from) {
  return v_table->move_constructor(placement, from);
}
inline mutable_void move_construct(is_move_constructor_v_table auto* v_table,
                                   mutable_void from) {
  return move_construct_at(v_table, allocate(v_table), from);
}
template <typename T>
inline void delete_(T v_table, mutable_void& data) noexcept {
  if (!data) return;
  if constexpr (!std::same_as<T, std::nullptr_t>) {
    assert(v_table);
    v_table->delete_(data);
    data = nullptr;
  }
}  // NOLINT
template <typename T>
inline void destruct(T v_table, mutable_void data) noexcept {
  if (!data) return;
  if constexpr (!std::same_as<T, std::nullptr_t>) {
    assert(v_table);
    v_table->destructor(data);
  }
}

template <typename U>
bool type_match(is_type_info_v_table auto* v_table);

template <typename U>
void check_type_match(is_type_info_v_table auto* v_table) {
  if (!type_match<U>(v_table)) throw type_mismatch_error("type mismatch");
}

template <typename Proxy>
struct proxy_trait;

template <typename Proxy>
struct basic_proxy_trait {
  inline static constexpr bool is_weak = false;
  inline static constexpr bool is_lifetime_bound = false;
  inline static constexpr bool is_object = true;
  inline static constexpr bool allow_any_default_constructibile = false;

  template <typename VTable>
  static constexpr bool is_compatible_with_v_table() {
    return true;
  }

  template <typename Rep>
  using proxy_impl = Proxy;

  static void move_to(auto& to, [[maybe_unused]] auto, auto&& from,
                      [[maybe_unused]] auto) {
    to = std::move(from);
  }

  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return true;
  }
  static void copy_construct_from(Proxy& to, [[maybe_unused]] void*,
                                  auto const& from, [[maybe_unused]] auto) {
    to = from;
  }

  static void destroy([[maybe_unused]] auto const& data,
                      [[maybe_unused]] void* v_table) {}
};

/// Requirements for a proxy type
/**
 */
template <typename E>
concept is_proxy = requires(E e) {
  typename proxy_trait<E>::void_t;
  typename proxy_trait<E>::static_dispatch_t;
  { proxy_trait<E>::is_constructibile_from_const } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_owner } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_weak } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_lifetime_bound } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_object } -> std::convertible_to<bool>;
  {
    proxy_trait<E>::allow_any_default_constructibile
  } -> std::convertible_to<bool>;

  // illustrated:
  // requires requires(v_table* required_v_table) {
  //   {
  //     proxy_trait<E>::get_proxy_ptr_in(e, required_v_table)
  //   } -> std::convertible_to<typename proxy_trait<E>::void_t>;
  // };
  // requires !proxy_trait<E>::is_owner ||
  //              requires(mutable_void from_data, a _v_table* v_table) {
  //                {
  //                  proxy_trait<E>::clone_from(from_data, v_table)
  //                };
  //              };
};

template <typename ProxyTrait, typename VTable>
concept can_copy_construct_from =
    ProxyTrait::template can_copy_construct_from<VTable>();

template <typename T>
struct is_type_class_impl : std::false_type {};
template <typename T>
struct is_type_class_impl<trait_class<T>> : std::true_type {};
template <typename T>
inline constexpr bool is_type_class =
    is_proxy<T> && is_type_class_impl<T>::value;

// template <typename Model>
// concept is_base_trait_model = true;

/// Requirements for a trait type
template <typename T>
concept has_v_table =
    std::derived_from<typename T::v_table_t, observeable_v_table>;

struct dynamic_castable;
struct dynamic_deletable;
struct dynamic_moveable;
struct dynamic_copyable;
template <typename Trait, is_proxy Proxy = typename Trait::default_proxy_t>
class any;

/// Requirements for a dynamic (i.e., type-erased) Proxy
template <typename Proxy>
concept is_dyn =
    is_proxy<Proxy> && voidness<typename proxy_trait<Proxy>::static_dispatch_t>;

template <typename I>
concept is_proxy_holder_impl = requires(I i) {
  typename I::proxy_t;
  typename I::proxy_trait_t;
};
template <typename I>
concept is_proxy_holder = is_proxy_holder_impl<std::decay_t<I>>;

template <typename I>
concept is_any_impl = is_proxy_holder_impl<I> &&
                      requires(I::proxy_t ed) { typename I::v_table_t; };
template <typename I>
concept is_any = is_any_impl<std::decay_t<I>>;

template <class E>
concept is_typed_any = is_any<E> && requires(E e) {
  typename E::proxy_trait_t;
  typename E::value_t;
  { E::is_const } -> std::convertible_to<bool>;
};

template <typename ConstructedWith, typename Proxy, typename BASE>
concept erased_constructibile_for =
    (!std::derived_from<std::remove_cvref_t<ConstructedWith>, BASE> &&
     !is_proxy<std::remove_cvref_t<ConstructedWith>> &&
     (!std::is_const_v<std::remove_reference_t<ConstructedWith>> ||
      proxy_trait<Proxy>::is_constructibile_from_const));

template <typename ConstructedWith, typename Proxy, typename Trait>
concept constructibile_for =
    (proxy_trait<Proxy>::template is_constructibile_from<
        ConstructedWith>::value) ||
    (erased_constructibile_for<ConstructedWith, Proxy, any<Trait, Proxy>> &&
     !is_proxy_holder<ConstructedWith> &&
     !is_typed_any<std::remove_cvref_t<ConstructedWith>>);

template <is_proxy Data>
using data_void = proxy_trait<Data>::void_t;

template <typename Proxy>
concept is_const_data = is_proxy<Proxy> && is_const_void<data_void<Proxy>>;

template <typename Proxy>
concept is_object_proxy = is_proxy<Proxy> && proxy_trait<Proxy>::is_object;

template <typename Proxy>
concept is_weak_data = is_proxy<Proxy> && proxy_trait<Proxy>::is_weak;

template <typename Proxy>
concept is_lifetime_bound =
    is_proxy<Proxy> && proxy_trait<Proxy>::is_lifetime_bound;

template <bool ToIsConst, bool FromIsConst, bool FromIsWeak>
concept const_correct_move_to_from =
    !FromIsWeak && ((ToIsConst == FromIsConst) || !FromIsConst);

constexpr inline bool is_const_correct_call_for_proxy_and_self(
    bool call_is_const, bool proxy_is_const, bool self_is_const, bool exact) {
  if (call_is_const) {
    if (exact) {
      return (call_is_const == proxy_is_const);
    } else {
      return true;
    }
  }

  if (proxy_is_const || self_is_const) return false;

  if (exact)
    return (call_is_const == proxy_is_const);
  else
    return true;
}

template <typename CALL, typename Proxy, bool SelfIsConst, bool Exact>
concept const_correct_call_for_proxy_and_self =
    is_object_proxy<Proxy> && !is_weak_data<Proxy> && voidness<CALL> &&
    is_proxy<Proxy> &&
    is_const_correct_call_for_proxy_and_self(
        is_const_void<CALL>, is_const_data<Proxy>, SelfIsConst, Exact);

template <is_proxy Proxy, typename From>
Proxy erased(From&& from) {
  return proxy_trait<Proxy>::erase(std::forward<From>(from));
}

template <is_proxy Proxy, typename ConstructedWith>
using unerased =
    proxy_trait<Proxy>::template unerased<std::decay_t<ConstructedWith>>;

template <is_proxy Proxy>
void const* get_proxy_ptr(Proxy const& vv, auto v_table)
  requires std::same_as<void const*, typename proxy_trait<Proxy>::void_t>
{
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}
template <is_proxy Proxy>
void* get_proxy_ptr(Proxy const& vv, auto v_table)
  requires std::same_as<void*, typename proxy_trait<Proxy>::void_t>
{
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}
template <is_proxy Proxy>
auto get_proxy_ptr(Proxy& vv, auto v_table) {
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}

template <typename U>
auto unchecked_unerase_cast(void const* p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(void* p) {
  return static_cast<U*>(p);
}

template <typename U, is_proxy Proxy>
auto unchecked_unerase_cast(Proxy const& o, auto v_table) {
  return unchecked_unerase_cast<U>(get_proxy_ptr(o, v_table));
}
template <typename U, is_proxy Proxy>
auto unchecked_unerase_cast(Proxy const& o, auto v_table)
  requires(!is_const_data<Proxy>)
{
  return unchecked_unerase_cast<U>(get_proxy_ptr(o, v_table));
}

template <typename U, is_proxy Proxy>
auto unerase_cast(Proxy const& o, auto* v_table) {
  check_type_match<U>(v_table);
  return unchecked_unerase_cast<U>(o, v_table);
}
template <typename U, is_proxy Proxy>
U const* unerase_cast_if(Proxy const& o, auto* v_table) {
  if (type_match<U>(v_table)) return unchecked_unerase_cast<U>(o, v_table);
  return nullptr;
}
template <typename U, is_proxy Proxy>
U* unerase_cast_if(Proxy const& o, auto* v_table)
  requires(!is_const_data<Proxy>)
{
  if (type_match<U>(v_table)) return unchecked_unerase_cast<U>(o, v_table);
  return nullptr;
}

// --------------------------------------------------------------------------------
// (un)erased data using_

static_assert(std::is_const_v<std::remove_reference_t<int const&>>);
static_assert(!std::is_const_v<std::remove_reference_t<int&>>);
static_assert(!std::is_const_v<std::remove_reference_t<int>>);

/// \defgroup proxies Proxies
/// \brief Proxies manage the storage in an \ref any

template <typename V>
struct proxy_trait<using_<V>> : basic_proxy_trait<using_<V>> {
  static constexpr bool allow_any_default_constructibile = true;

  template <typename Rep>
  using proxy_impl = using_<Rep>;
  using void_t = std::conditional_t<std::is_const_v<std::remove_reference_t<V>>,
                                    const_void, mutable_void>;
  using static_dispatch_t = V;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = std::is_constructible_v<
        V, ConstructedWith>;  // && !is_any<ConstructedWith>;
  };
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val,
                               [[maybe_unused]] observeable_v_table* v_table) {
    return val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  static auto construct_in_place(V&& v) { return std::move(v); }
  template <typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return V{std::forward<Args>(args)...};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return using_<V>{std::forward<Vx>(v)};
  }
};

template <typename V>
struct proxy_trait<using_cref<V>> : basic_proxy_trait<using_cref<V>> {
  static constexpr bool allow_any_default_constructibile = true;

  template <typename Rep>
  using proxy_impl = using_cref<Rep>;
  using void_t = std::conditional_t<std::is_const_v<std::remove_reference_t<V>>,
                                    const_void, mutable_void>;
  using static_dispatch_t = V;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = std::is_constructible_v<
        V, ConstructedWith>;  // && !is_any<ConstructedWith>;
  };
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val,
                               [[maybe_unused]] observeable_v_table* v_table) {
    return val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  static auto construct_in_place(V&& v) { return std::move(v); }
  template <typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return V{std::forward<Args>(args)...};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return using_cref<V>{std::forward<Vx>(v)};
  }
};

template <typename Type>
struct proxy_trait<trait_class<Type>> : basic_proxy_trait<trait_class<Type>> {
  using void_t = const_void;
  using static_dispatch_t = Type;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_object = false;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = true;
  };
  static constexpr bool is_owner = false;
  static constexpr bool allow_any_default_constructibile = true;

  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {}

  static auto get_proxy_ptr_in([[maybe_unused]] auto& val,
                               [[maybe_unused]] auto* v_table) {
    return nullptr;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  static auto construct_in_place(auto) {}
  template <typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {}
  template <typename Vx>
  static auto erase([[maybe_unused]] Vx&& v) {
    return trait_class<Vx>{};
  }
};

/// A Proxy for mixing std::variant and type erasure
/// Use this when some, at compile time, known types are dispatched in a hot
/// path. Put these types plus an any (with the specific \ref trait and a
/// dynamic proxy) in a std::variant. This variant is then used with a \ref
/// using_ proxy and the same \ref trait as before. See \ref make_vany
/// So the dispatch for the types in the variant is done internally with a
/// std::visit, and all other types are dispatched via their v-Table. Now you
/// know where the Any++ logo has its origin.
/// \ingroup proxies
template <template <typename> typename Any, is_proxy Proxy, typename... Types>
using vany_variant = std::variant<Any<Proxy>, Types...>;

/// A factory type to direct get the any for the vany
template <template <typename> typename Any, is_proxy Proxy, typename... Types>
using make_vany = Any<using_<vany_variant<Any, Proxy, Types...>>>;

template <typename VanyVariant>
struct vany_variant_trait {
  using vany_variant_val = VanyVariant;
  using vany_variant = typename VanyVariant::value_t;
  template <typename... Types>
  struct concrete_variant_impl;
  template <typename First, typename... Types>
  struct concrete_variant_impl<std::variant<First, Types...>> {
    using type = std::variant<Types...>;
  };
  using concrete_variant = typename concrete_variant_impl<vany_variant>::type;
  using any_in_variant = typename std::variant_alternative_t<0, vany_variant>;
};

template <typename Vany>
struct vany_type_trait {
  using vany = Vany;
  using vany_variant = typename Vany::proxy_t;
  using concrete_variant =
      typename vany_variant_trait<vany_variant>::concrete_variant;
  using any_in_variant =
      typename vany_variant_trait<vany_variant>::any_in_variant;
};

template <template <typename> typename Any, is_proxy Proxy, typename... Types>
struct proxy_trait<using_<vany_variant<Any, Proxy, Types...>>>
    : basic_proxy_trait<using_<vany_variant<Any, Proxy, Types...>>> {
  using vany_variant_t = vany_variant<Any, Proxy, Types...>;
  using void_t = typename proxy_trait<Proxy>::void_t;
  using static_dispatch_t = vany_variant_t;
  static constexpr bool is_constructibile_from_const =
      proxy_trait<Proxy>::is_constructibile_from_const;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value =
        std::is_constructible_v<vany_variant_t, ConstructedWith>;
  };
  static constexpr bool is_owner = proxy_trait<Proxy>::is_owner;
  static constexpr bool is_weak =
      proxy_trait<Proxy>::is_weak;  // cppcheck-suppress
                                    // duplInheritedMember
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val, [[maybe_unused]] auto* v_table) {
    return val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static vany_variant_t construct_in_place(V&& v) {
    return using_<vany_variant_t>{std::forward<V>(v)};
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return using_<vany_variant_t>{T{std::forward<Args>(args)...}};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return using_<vany_variant_t>{std::forward<Vx>(v)};
  }
};

// --------------------------------------------------------------------------------
// erased data observer

template <voidness Voidness>
struct observer_trait : basic_proxy_trait<Voidness> {
  using void_t = Voidness;
  using static_dispatch_t = void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;
  static constexpr bool is_lifetime_bound = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = false;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }
  static void move_to(Voidness& to, [[maybe_unused]] auto, Voidness from,
                      [[maybe_unused]] auto) {
    to = from;
  }

  static Voidness get_proxy_ptr_in(const auto& ptr,
                                   [[maybe_unused]] auto* v_table) {
    return ptr;
  }

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
    return static_cast<Voidness>(&v);
  }
  template <typename V>
  static auto erase(const V& v)
    requires(is_const)
  {
    return static_cast<Voidness>(&v);
  }
};

template <>
struct proxy_trait<cref> : observer_trait<cref> {};
template <>
struct proxy_trait<mutref> : observer_trait<mutref> {};

static_assert(proxy_trait<cref>::is_const);
static_assert(!proxy_trait<mutref>::is_const);
static_assert(is_proxy<cref>);
static_assert(is_proxy<mutref>);
static_assert(is_proxy<mutref>);
static_assert(is_proxy<cref>);

// --------------------------------------------------------------------------------
// erased data unique

/// Proxy to manage the captured object via std::unique_ptr-like smart pointer
/// * If you pass a std::unique_ptr to the any constructor, this pointer will
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
    delete_(v_table_to, old);
  }

  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return false;
  }

  static void* get_proxy_ptr_in(const auto& ptr,
                                [[maybe_unused]] auto* v_table) {
    return ptr.ptr;
  }

  static void destroy(unique& u, is_delete_v_table auto* v_table) {
    assert(v_table || !u.ptr);
    if (v_table) delete_(v_table, u.ptr);
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
      requires (!requires { typename std::decay_t<V>::deleter_type; })
  static auto erase(V&& v) {
      return unique{ new V{std::move(v)} };
  }
};

static_assert(is_proxy<unique>);

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
/// Proxy to manage the captured object via \c std::weak_ptr.
/// Assign or copy construct it from a  \c any<shared>
/// \ingroup proxies
using weak = std::weak_ptr<void const>;

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
      requires (!requires { typename std::decay_t<V>::weak_type; })
  static auto erase(V&& v) {
      return std::make_shared<std::decay_t<V>>(std::forward<V>(v));
  }
};

template <>
struct proxy_trait<weak> : basic_proxy_trait<weak> {
  using void_t = void const*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = false;
  static constexpr bool is_weak =  // NOLINT([duplInheritedMember)
      true;                        // cppcheck-suppress duplInheritedMember
  static constexpr bool allow_any_default_constructibile = true;

  static auto clone_from([[maybe_unused]] const_void data_ptr,  // NOLINT
                         [[maybe_unused]] auto* v_table) {
    return weak{};
  }

  static void const* get_proxy_ptr_in([[maybe_unused]] const auto& ptr,
                                      [[maybe_unused]] auto* v_table) {
    return nullptr;
  }

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
  static auto erase(std::shared_ptr<V> const& v) {
    return weak{static_pointer_cast<void const>(v)};
  }
  template <typename V>
  static auto erase(std::weak_ptr<V> const& v) {
    return erase(v.lock());
  }
};

static_assert(is_proxy<shared>);
static_assert(is_proxy<weak>);

// --------------------------------------------------------------------------------
// erased data cow

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
        auto holder_size =
            sizeof(cow::holder<>) - sizeof(void*) + model_size(v_table).size;
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

// --------------------------------------------------------------------------------
// erased data val

template <typename Model>
constexpr inline model_size_t compute_model_size() {
  return {.size = sizeof(Model),
          .trivial = std::is_trivially_default_constructible_v<Model> &&
                     std::is_trivially_copyable_v<Model>};
}

template <bool Trivial, std::size_t SmallObjectSize>
struct local_data : std::array<std::byte, SmallObjectSize> {
  static constexpr inline bool is_trivial = Trivial;
};

/// \brief Proxy to manage the captured object as value with small object
/// optimization
/// * If you forward an object to any constructor, this object will be
/// forwarded to the allocated storage.
/// * To pass an object as second parameter, with the std::in_place tag as
/// first, has the same behavior as above
/// * If you pass as first parameter std::in_place_type<...>, the object will
/// be constructed in place in the allocated memory with the other arguments
/// forwarded
/// \ingroup proxies
template <typename Nullable = std::false_type,
          std::size_t SmallObjectSize = small_object_size>
struct val {
  union data_union {
    data_union(cow::holder_base* cow_holder) : heap(cow{cow_holder}) {}
    template <typename T, typename... Args>
    data_union(std::in_place_type_t<T>, Args&&... args) {
      local = {};
      auto location = static_cast<T*>(static_cast<mutable_void>(local.data()));
      std::construct_at<T>(location, std::forward<Args>(args)...);
    }
    data_union(data_union const& other) noexcept { trivial = other.trivial; }
    ~data_union() {
      // do nothing, the destruction  is done by the proxy_trait<val>::destroy
    }
    cow heap;
    local_data<false, SmallObjectSize> local;
    local_data<true, SmallObjectSize> trivial;
  } data;
  mutable_void ptr_ = nullptr;

  val() : data{nullptr}, ptr_{nullptr} {}
  ~val() {}

  template <typename T, typename... Args>
  val(std::in_place_type_t<T> t, Args&&... args)
    requires(sizeof(T) <= SmallObjectSize)
      : data(t, std::forward<Args>(args)...) {
    ptr_ = data.local.data();
  }
  template <typename T, typename... Args>
  val(std::in_place_type_t<T>, Args&&... args)
    requires(sizeof(T) > SmallObjectSize)
      : data{new cow::holder<T>(std::forward<Args>(args)...)} {
    ptr_ = data.heap.data_ptr();
  }
};

using nullable_val = val<std::true_type>;

template <std::size_t SmallObjectSize, typename V>
auto visit_value(auto&& visitor, V&& v, model_size_t size) -> decltype(auto) {
  if (size.size > SmallObjectSize) {
    return std::forward<decltype(visitor)>(visitor)(
        std::forward<V>(v).data.heap);
  } else if (!size.trivial) {
    return std::forward<decltype(visitor)>(visitor)(
        std::forward<V>(v).data.local);
  }
  assert(size.trivial);
  return std::forward<decltype(visitor)>(visitor)(
      std::forward<V>(v).data.trivial);
};

template <typename Nullable, std::size_t SmallObjectSize, typename V2>
auto visit_value(auto&& visitor, val<Nullable, SmallObjectSize>& v1,
                 model_size_t size1, V2&& v2, model_size_t size2)
    -> decltype(auto) {
  if (size1.size > SmallObjectSize) {
    if (size2.size > SmallObjectSize) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.heap, std::forward<V2>(v2).data.heap);
    } else if (!size2.trivial) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.heap, std::forward<V2>(v2).data.local);
    }
    assert(size2.trivial);
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.heap, std::forward<V2>(v2).data.trivial);
  } else if (!size1.trivial) {
    if (size2.size > SmallObjectSize) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.local, std::forward<V2>(v2).data.heap);
    } else if (!size2.trivial) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.data.local, std::forward<V2>(v2).data.local);
    }
    assert(size2.trivial);
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.local, std::forward<V2>(v2).data.trivial);
  }
  assert(size1.trivial);
  if (size2.size > SmallObjectSize) {
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.trivial, std::forward<V2>(v2).data.heap);
  } else if (!size2.trivial) {
    return std::forward<decltype(visitor)>(visitor)(
        v1.data.trivial, std::forward<V2>(v2).data.local);
  }
  assert(size2.trivial);
  return std::forward<decltype(visitor)>(visitor)(
      v1.data.trivial, std::forward<V2>(v2).data.trivial);
};

template <typename Nullable, std::size_t SmallObjectSize>
struct proxy_trait<val<Nullable, SmallObjectSize>>
    : basic_proxy_trait<val<Nullable, SmallObjectSize>> {
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
  static constexpr bool allow_any_default_constructibile = Nullable::value;

  template <typename VTable>
  static constexpr bool is_compatible_with_v_table() {
    return is_copy_constructor_v_table<VTable> &&
           is_move_constructor_v_table<VTable> && is_destructor_v_table<VTable>;
  }

  static auto clone_from([[maybe_unused]] mutable_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    assert(v_table);
    val<Nullable, SmallObjectSize> v;
    v.ptr_ = visit_value<SmallObjectSize>(
        overloads{
            [&](cow& heap) {
              heap.emplace(proxy_trait<cow>::clone_from(data_ptr, v_table));
              return heap.data_ptr();
            },
            [&]<bool Trivial>(local_data<Trivial, SmallObjectSize>& local) {
              auto local_data = static_cast<mutable_void>(local.data());
              copy_construct_at(v_table, local_data, data_ptr);
              return local_data;
            }},
        v, v_table->model_size);
    return v;
  }

  static void move_to(val<Nullable, SmallObjectSize>& to,
                      [[maybe_unused]] auto v_table_to,
                      val<Nullable, SmallObjectSize>&& from,
                      [[maybe_unused]] auto* v_table_from) {
    if (v_table_from == nullptr && v_table_to == nullptr) return;
    to.ptr_ = visit_value(
        overloads{
            [&](cow& t, cow& f) {
              proxy_trait<cow>::move_to(t, v_table_to, std::move(f),
                                        v_table_from);
              return t.data_ptr();
            },
            [&](cow& t, local_data<false, SmallObjectSize>& f) {
              proxy_trait<cow>::destroy(t, v_table_to);
              return v_table_from->move_constructor(to.data.local.data(),
                                                    f.data());
            },
            [&](cow& t, local_data<true, SmallObjectSize>& f) {
              proxy_trait<cow>::destroy(t, v_table_to);
              to.data.trivial = std::move(f);
              return (mutable_void)&to.data.trivial;
            },
            [&](local_data<false, SmallObjectSize>& t, cow& f) {
              destruct(v_table_to, t.data());
              proxy_trait<cow>::move_to(to.data.heap, nullptr, std::move(f),
                                        nullptr);
              return to.data.heap.data_ptr();
            },
            [&](local_data<false, SmallObjectSize>& t,
                local_data<false, SmallObjectSize>& f) {
              destruct(v_table_to, t.data());
              return v_table_from->move_constructor(to.data.local.data(),
                                                    f.data());
            },
            [&](local_data<false, SmallObjectSize>& t,
                local_data<true, SmallObjectSize>& f) {
              destruct(v_table_to, t.data());
              to.data.trivial = std::move(f);
              return (mutable_void)&to.data.trivial;
            },
            [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t, cow& f) {
              proxy_trait<cow>::move_to(to.data.heap, nullptr, std::move(f),
                                        nullptr);
              return to.data.heap.data_ptr();
            },
            [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t,
                local_data<false, SmallObjectSize>& f) {
              return v_table_from->move_constructor(to.data.local.data(),
                                                    f.data());
            },
            [&](local_data<true, SmallObjectSize>& t,
                local_data<true, SmallObjectSize>& f) {
              t = std::move(f);
              return (mutable_void)&t;
            }},
        to, model_size(v_table_to), from, v_table_from->model_size);
    from.ptr_ = nullptr;
  }
  // TODO implement move from unique
  // static void move_to(unique& to, auto* to_v_table, val<>&& v,
  //                    auto* v_table) {
  //  assert(v_table);
  //  auto data_ptr =
  //      visit_value(overloads{[&](heap_data& heap) { return heap.release();
  //      },
  //                            [&]<bool Trivial>(local_data<Trivial>& local)
  //                            {
  //                              return move_construct(v_table,
  //                              local.data());
  //                            }},
  //                  v, v_table->model_size);
  //  proxy_trait<unique>::move_to(to, to_v_table, unique{data_ptr}, v_table);
  //}

  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return is_copy_constructor_v_table<VTable>;
  }
  static void copy_construct_from(val<Nullable, SmallObjectSize>& to,
                                  auto to_v_table,
                                  val<Nullable, SmallObjectSize> const& from,
                                  auto* from_v_table) {
    if (!from_v_table) return;
    to.ptr_ = visit_value(
        overloads{[&](cow& t, cow const& from_data) {
                    proxy_trait<cow>::copy_construct_from(
                        t, to_v_table, from_data, from_v_table);
                    return t.data_ptr();
                  },
                  [&](cow& t, local_data<false, SmallObjectSize> const& f) {
                    proxy_trait<cow>::destroy(t, to_v_table);
                    return from_v_table->copy_constructor(to.data.local.data(),
                                                          f.data());
                  },
                  [&](cow& t, local_data<true, SmallObjectSize> const& f) {
                    proxy_trait<cow>::destroy(t, to_v_table);
                    to.data.trivial = f;
                    return (mutable_void)&to.data.trivial;
                  },
                  [&](local_data<false, SmallObjectSize>& t, cow const& f) {
                    destruct(to_v_table, t.data());
                    proxy_trait<cow>::assign(to.data.heap, f);
                    return to.data.heap.data_ptr();
                  },
                  [&](local_data<false, SmallObjectSize>& t,
                      local_data<false, SmallObjectSize> const& f) {
                    destruct(to_v_table, t.data());
                    return from_v_table->copy_constructor(to.data.local.data(),
                                                          f.data());
                  },
                  [&](local_data<false, SmallObjectSize>& t,
                      local_data<true, SmallObjectSize> const& f) {
                    destruct(to_v_table, t.data());
                    to.data.trivial = f;
                    return (mutable_void)&to.data.trivial;
                  },
                  [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t,
                      cow const& f) {
                    proxy_trait<cow>::assign(to.data.heap, f);
                    return to.data.heap.data_ptr();
                  },
                  [&]([[maybe_unused]] local_data<true, SmallObjectSize>& t,
                      local_data<false, SmallObjectSize> const& f) {
                    return from_v_table->copy_constructor(to.data.local.data(),
                                                          f.data());
                  },
                  [&](local_data<true, SmallObjectSize>& t,
                      local_data<true, SmallObjectSize> const& f) {
                    t = f;
                    return (mutable_void)&t;
                  }},
        to, model_size(to_v_table), from, from_v_table->model_size);
  }

  static void destroy(val<Nullable, SmallObjectSize>& v, auto* v_table) {
    visit_value<SmallObjectSize>(
        overloads{
            [&](cow& heap) {
              if (v_table) proxy_trait<cow>::destroy(heap, v_table);
            },
            [&](local_data<false, SmallObjectSize>& local) {
              if (v_table) destruct(v_table, local.data());
            },
            [&]([[maybe_unused]] local_data<true, SmallObjectSize>& local) {}},
        v, model_size(v_table));
  }

  static void* get_proxy_ptr_in(val<Nullable, SmallObjectSize> const& v,
                                [[maybe_unused]] auto* v_table) {
    return v.ptr_;
  }
  static void* get_proxy_ptr_in(val<Nullable, SmallObjectSize>& v,
                                auto* v_table) {
    return visit_value<SmallObjectSize>(
        overloads{
            [&](cow& heap) {
              return v.ptr_ = proxy_trait<cow>::get_proxy_ptr_in(heap, v_table);
            },
            [&]<bool Trivial>(
                [[maybe_unused]] local_data<Trivial, SmallObjectSize>& local) {
              return v.ptr_;
            }},
        v, model_size(v_table));
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return val<Nullable, SmallObjectSize>(std::in_place_type<std::decay_t<V>>,
                                          std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return val<Nullable, SmallObjectSize>(std::in_place_type<T>,
                                          std::forward<Args>(args)...);
  }
  template <typename ConstructedWith>
  static auto erase(ConstructedWith&& v) {
    return val<Nullable, SmallObjectSize>(
        std::in_place_type<std::decay_t<ConstructedWith>>,
        std::forward<ConstructedWith>(v));
  }
};

static_assert(is_proxy<val<>>);
static_assert(is_object_proxy<val<>>);

// --------------------------------------------------------------------------------
// meta data

class meta_data;

template <typename TYPE>
auto& runtime_implementation();

#ifdef ANY_DLL_MODE
template <typename T>
meta_data& get_meta_data();
#else
template <typename T>
meta_data& get_meta_data() {
  return runtime_implementation<std::decay_t<T>>();
}
#endif

template <typename Proxy>
using static_dispatch_t = typename proxy_trait<Proxy>::static_dispatch_t;

template <typename Proxy, typename Trait>
using proxy_model_map_t =
    typename Trait::template model_map<static_dispatch_t<Proxy>>;

template <typename Proxy, typename Trait>
using proxy_deduced_type_t =
    typename proxy_model_map_t<Proxy, Trait>::deduced_type;

template <typename Trait>
using v_table_t = typename Trait ::v_table_t;

template <typename Trait>
using v_table_deduced_type_t = typename v_table_t<Trait>::deduced_type;

template <typename Proxy, typename Trait>
struct v_table_holder;

template <typename Proxy, typename Trait>
  requires(!is_dyn<Proxy>)
struct v_table_holder<Proxy, Trait> : proxy_deduced_type_t<Proxy, Trait> {
  struct v_table_t {};

  v_table_holder() = default;
  explicit v_table_holder(v_table_t*) {}
  static void set_v_table_ptr(auto) {}
  static auto get_v_table_ptr() { return nullptr; }
  template <typename...>
  static void init_v_table() {}
  static auto release_v_table() { return nullptr; }
};

template <typename Proxy, typename Trait>
  requires is_dyn<Proxy>
struct v_table_holder<Proxy, Trait> : v_table_deduced_type_t<Trait> {
 public:
  using v_table_t = Trait::v_table_t;

 private:
  v_table_t* v_table_ = nullptr;

 public:
  v_table_holder() = default;
  explicit v_table_holder(v_table_t* v_table) : v_table_(v_table) {}
  void set_v_table_ptr(v_table_t* v_table) { v_table_ = v_table; }
  // cppcheck-suppress-begin [functionConst, functionStatic]
  auto get_v_table_ptr() const { return v_table_; }
  // cppcheck-suppress-end [functionConst, functionStatic]
  template <typename ProxyImpl, typename Concrete>
  void init_v_table() {
    v_table_ =
        v_table_instance<v_table_t, anyxx::unerased<ProxyImpl, Concrete>>();
  }
  auto release_v_table() { return std::exchange(v_table_, nullptr); }
};

struct cast_error {
  std::type_info const &to, &from;
};

template <typename VTable, typename Concrete>
auto bind_v_table_to_meta_data() {
  auto v_table = v_table_instance<VTable, Concrete>();
  get_meta_data<Concrete>().register_v_table(v_table);
  return v_table;
}

template <typename U>
bool type_match(is_type_info_v_table auto* v_table) {
  return *v_table->type_info_ == typeid(std::decay_t<U>);
}

// --------------------------------------------------------------------------------
// borrow erased data

template <is_proxy To, is_proxy From>
struct borrow_trait;

template <typename To, typename From, typename FromVTable>
concept proxy_borrowable_from =
    is_proxy<From> && is_proxy<To> && requires(From f, FromVTable* v_table) {
      { borrow_trait<To, From>{}(f, v_table) } -> std::same_as<To>;
    };

template <typename To, typename From, typename FromVTable>
  requires proxy_borrowable_from<To, From, FromVTable>
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

// --------------------------------------------------------------------------------
// clone erased data

template <is_proxy To>
struct can_copy_to;

template <typename To>
concept cloneable_to = is_proxy<To> && proxy_trait<To>::is_owner;

template <is_proxy To, is_proxy From>
  requires cloneable_to<To>
To clone_to(From const& from, auto v_table) {
  return proxy_trait<To>::clone_from(get_proxy_ptr(from, v_table), v_table);
}

// --------------------------------------------------------------------------------
// move erased data

template <typename To, typename From>
inline static bool constexpr can_move_to_from = false;

template <typename To, typename From>
concept moveable_from =
    is_proxy<From> && is_proxy<To> && can_move_to_from<To, From>;

template <is_proxy X>
inline static bool constexpr can_move_to_from<X, X> = true;

template <>
inline bool constexpr can_move_to_from<shared, unique> = true;

template <>
inline bool constexpr can_move_to_from<weak, shared> = true;

template <voidness To, voidness From>
  requires const_correct_move_to_from<is_const_void<To>, is_const_void<From>,
                                      is_weak_data<From>>
inline static bool constexpr can_move_to_from<To, From> = true;

template <is_proxy To, is_proxy From>
  requires moveable_from<To, std::decay_t<From>>
void move_to(To& to, auto to_v_table, From&& from, auto from_v_table) {
  return proxy_trait<From>::move_to(to, to_v_table, std::move(from),
                                    from_v_table);
}

template <typename Proxy, typename Trait>
concept is_proxy_compatible_with_trait =
    is_proxy<Proxy> && has_v_table<Trait> &&
    proxy_trait<Proxy>::template is_compatible_with_v_table<
        typename Trait::v_table_t>();

template <typename DerivedAny, typename BaseAny>
constexpr bool is_any_derived_from_v = false;

template <typename DerivedAny, typename BaseAny>
concept is_any_derived_from = is_any_derived_from_v<DerivedAny, BaseAny>;

template <typename TraitDerived, is_proxy ProxyDerived, typename TraitBase,
          is_proxy ProxyBase>
constexpr bool is_any_derived_from_v<any<TraitDerived, ProxyDerived>,
                                     any<TraitBase, ProxyBase>> =
    std::derived_from<typename any<TraitDerived, ProxyDerived>::v_table_t,
                      typename any<TraitBase, ProxyBase>::v_table_t>;

/// \brief The core class template to control dispatch for external
/// polymorphism
///
/// To control the behavior, `any` provides two template parameters: \ref
/// Proxy and \ref Trait. Imagine this as a combination of a `std::any` and
/// several `std::function`s.
///
/// With the Proxy template parameter, you control whether this `any` behaves
/// like a copying function, a move-only function, a reference function, or if
/// the target object is captured concretely inside of `any`.
///
/// With the Trait template parameter, you specify the member functions of a
/// captured object which can be invoked on this `any`.
///
/// \tparam Proxy Specifies the lifetime of the captured object. Any++
/// provides
/// \ref using_, \ref cref, \ref mutref, \ref shared, \ref weak, \ref unique,
/// and
/// \ref value. All Proxy classes must conform to the \ref is_proxy concept.
/// \tparam Trait Specifies the functionality of this any. A class of this
/// type is normally provided via a \ref TRAIT or \ref ANY macro. See there
/// for examples. If the proxy is dynamic (i.e., type erased), the Trait must
/// conform to the \ref has_v_table concept (that means: must provide a
/// v-Table).
template <typename Trait, is_proxy Proxy>
class ANYXX_USE_EBO any : public v_table_holder<Proxy, Trait>, public Trait {
 public:
  using proxy_t = Proxy;
  using proxy_trait_t = proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_holder_t = v_table_holder<Proxy, Trait>;
  using trait_t = Trait;
  using v_table_t = typename v_table_holder_t::v_table_t;
  using T = proxy_trait_t::static_dispatch_t;
  using model_map_t = typename trait_t::template static_dispatch_map_t<T>;
  using rep_type = typename model_map_t::rep_type;
  using proxy_impl_t = typename proxy_trait_t::template proxy_impl<rep_type>;
  static constexpr bool is_dyn = anyxx::is_dyn<Proxy>;
  // static_assert(is_proxy_compatible_with_trait<Proxy, Trait>,
  //               "If the proxy is dynamic, the trait must provide a v-table "
  //               "compatible with the proxy.");
  //  static_assert(!is_dyn || has_v_table<Trait>); has issues in clang...

 protected:
  proxy_impl_t proxy_{};

 public:
  any()
    requires proxy_trait_t::allow_any_default_constructibile
  {}

  // cppcheck-suppress-begin noExplicitConstructor
  /// Type-erasing constructor for lifetime owning proxies. The concrete
  /// behavior is controlled by the proxy via its corresponding \ref
  /// proxy_trait. See \ref using_, \ref shared, \ref weak, \ref unique, and
  /// \ref value.
  template <typename ConstructedWith>
  explicit(false) any(ConstructedWith&& constructed_with)  // NOLINT
    requires constructibile_for<ConstructedWith, proxy_impl_t, trait_t> &&
             (!std::same_as<any, std::decay_t<ConstructedWith>>) &&
             (!is_lifetime_bound<proxy_impl_t>)
      : proxy_(
            erased<proxy_t>(std::forward<ConstructedWith>(constructed_with))) {
    v_table_holder_t::template init_v_table<Proxy, ConstructedWith>();
  }
  /// Type-erasing constructor for borrowing proxies. The concrete behavior is
  /// controlled by the proxy via its corresponding \ref proxy_trait. See \ref
  /// cref, \ref mutref
  template <typename ConstructedWith>
  explicit(false)
      any(ConstructedWith&& constructed_with LIFETIMEBOUND)  // NOLINT
    requires constructibile_for<ConstructedWith, proxy_impl_t, trait_t> &&
             (!std::same_as<any, std::decay_t<ConstructedWith>>) &&
             (is_lifetime_bound<proxy_impl_t>)
      : proxy_(
            erased<proxy_t>(std::forward<ConstructedWith>(constructed_with))) {
    v_table_holder_t::template init_v_table<proxy_impl_t, ConstructedWith>();
  }
  // cppcheck-suppress-end noExplicitConstructor
  /// Type-erasing constructor. The concrete behavior is controlled by the
  /// proxy. The value `v` will be forwarded to the managed storage.
  /// \tparam V The type of the object to be forwarded into the managed
  /// storage. Usually deduced.
  /// \param std::in_place_t Tag to select in-place construction. (On call
  /// site, use `std::in_place`.)
  /// \param v The object to be forwarded into the managed storage.
  template <typename V>
    requires(!is_lifetime_bound<Proxy>)
  any(std::in_place_t, V&& v)
      : proxy_(
            proxy_trait<proxy_impl_t>::construct_in_place(std::forward<V>(v))) {
    v_table_holder_t::template init_v_table<proxy_impl_t, V>();
  }
  /// Type erasing constructor, the concrete behavior is controled by the
  /// proxy
  /// \param std::in_place_type_t<T> Tag to select in-place construction of
  /// `T`
  /// \param args The arguments to construct the object of type T with, will
  /// be forwarded.
  template <typename T, typename... Args>
    requires(!is_lifetime_bound<Proxy>)
  any(std::in_place_type_t<T>, Args&&... args)
      : proxy_(proxy_trait_t::template construct_type_in_place<T>(
            std::forward<Args>(args)...)) {
    v_table_holder_t::template init_v_table<proxy_impl_t, T>();
  }

  ~any() {
    proxy_trait_t::destroy(proxy_, v_table_holder_t::get_v_table_ptr());
  }

  any(const any& other)
    requires(is_dyn && can_copy_construct_from<proxy_trait_t, v_table_t>)
      : v_table_holder_t(other.get_v_table_ptr()) {
    proxy_trait_t::copy_construct_from(proxy_, nullptr, other.proxy_,
                                       other.get_v_table_ptr());
  }
  any(const any& other)
    requires(!is_dyn && can_copy_construct_from<proxy_trait_t, v_table_t>)
      : v_table_holder_t(other.get_v_table_ptr()), proxy_(other.proxy_) {}
  any& operator=(any const& other)
    requires(can_copy_construct_from<proxy_trait_t, v_table_t>)
  {
    if (this == &other) return *this;
    auto const v_table_ptr = v_table_holder_t::get_v_table_ptr();
    proxy_trait_t::copy_construct_from(proxy_, v_table_ptr, other.proxy_,
                                       other.get_v_table_ptr());
    v_table_holder_t::set_v_table_ptr(other.get_v_table_ptr());
    return *this;
  }

  template <is_any Other>
  explicit(false) any(const Other& other)  // NOLINT(noExplicitConstructor)
    requires(proxy_borrowable_from<proxy_t, typename Other::proxy_t,
                                   typename Other::v_table_t> &&
             (!anyxx::is_dyn<Proxy> ||
              is_any_derived_from<Other, any>))
      : v_table_holder_t(other.get_v_table_ptr()),
        proxy_(borrow_proxy_as<Proxy>(other.proxy_, other.get_v_table_ptr())) {}
  template <is_any Other>
  any& operator=(Other const& other)
    requires(proxy_borrowable_from<proxy_t, typename Other::proxy_t,
                                   typename Other::v_table_t> &&
             (!anyxx::is_dyn<Proxy> ||
              is_any_derived_from<Other, any>))
  {
    v_table_holder_t::set_v_table_ptr(other.get_v_table_ptr());
    proxy_ = borrow_proxy_as<Proxy>(other.proxy_, other.get_v_table_ptr());
    return *this;
  }

  template <is_proxy OtherErasedData>
    requires(moveable_from<proxy_t, OtherErasedData>)
  explicit any(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_holder_t(v_table) {
    proxy_trait_t::move_to(proxy_, nullptr, std::move(proxy), v_table);
  }
  template <is_proxy OtherErasedData>
    requires(moveable_from<proxy_t, OtherErasedData> && !is_dyn)
  explicit any(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_holder_t(v_table), proxy_(std::move(proxy)) {}
  template <is_any Other>
  explicit(false) any(Other&& other) noexcept  // NOLINT(noExplicitConstructor)
    requires(moveable_from<proxy_t, typename Other::proxy_t> &&
             (!anyxx::is_dyn<Proxy> ||
              is_any_derived_from<Other, any>))
      : any(std::move(other.proxy_), other.release_v_table()) {}
  template <is_any Other>
  any& operator=(Other&& other) noexcept
    requires(moveable_from<proxy_t, typename Other::proxy_t> &&
             (!anyxx::is_dyn<Proxy> ||
              is_any_derived_from<Other, any>))
  {
    proxy_trait_t::move_to(proxy_, v_table_holder_t::get_v_table_ptr(),
                           std::move(other.proxy_), other.get_v_table_ptr());
    v_table_holder_t::set_v_table_ptr(other.release_v_table());
    return *this;
  }

  template <is_any Friend>
  friend inline auto& get_proxy(Friend const& any);
  template <is_any Friend>
  friend inline auto& get_proxy(Friend& any);
  template <is_any Friend>
  friend inline decltype(auto) move_proxy(Friend&& any);
  template <is_any Friend>
  friend inline auto get_proxy_ptr_const(Friend const& any);
  template <is_any Friend>
  friend inline auto get_proxy_ptr(Friend&& any);

  template <typename OtherTrait, is_proxy Other>
  friend class any;

  template <typename Friend>
    requires is_any<Friend> && Friend::is_dyn
  friend inline auto get_v_table(Friend const& any);

  template <is_any To, is_any From>
  friend inline To unchecked_downcast_to(From from)
    requires(
        is_any_derived_from<To, From>);

  explicit operator bool() const
    requires proxy_trait_t::allow_any_default_constructibile
  {
    if constexpr (!voidness<typename proxy_trait_t::static_dispatch_t>) {
      if constexpr (is_type_class<proxy_t>) {
        return true;
      } else {
        return proxy_.value_;
      }
    } else {
      auto p = get_proxy_ptr(*this);
      return p != nullptr;
    }
  }

  template <typename Self>
  Self operator++(this Self& self, int)
    requires(is_dyn && is_op_pre_increment_v_table<v_table_t>) ||
            (!is_dyn && is_op_pre_increment_model_map<model_map_t, T>)

  {
    auto r = self;
    ++(self);
    return r;
  }

  template <typename Self>
  Self& operator++(this Self&& self)
    requires is_dyn && is_op_pre_increment_v_table<v_table_t>
  {
    get_v_table(self)->op_pre_increment(
        get_proxy_ptr(std::forward<Self>(self)));
    return std::forward<Self>(self);
  }

  template <typename Self>
  Self& operator++(this Self&& self)
    requires(!is_dyn && is_op_pre_increment_model_map<model_map_t, T>)
  {
    model_map_t{}.op_pre_increment(get_proxy_value(std::forward<Self>(self)));
    return std::forward<Self>(self);
  }
};

template <is_any Any>
inline auto& get_proxy(Any const& any) {
  return any.proxy_;
}
template <is_any Any>
inline auto& get_proxy(Any& any) {
  return any.proxy_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::is_dyn)
inline auto& get_proxy_value(Any&& any) {
  return get_proxy(std::forward<Any>(any)).value_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::is_dyn)
inline auto const& get_proxy_value(Any const& any) {
  return get_proxy(any).value_;
}
template <is_any Any>
inline decltype(auto) move_proxy(Any&& any) {
  return std::move(any.proxy_);
}
template <is_any Any>
inline auto get_proxy_ptr_const(Any const& any) {
  return get_proxy_ptr(get_proxy(any), get_v_table(any));
}
template <is_any Any>
inline auto get_proxy_ptr(Any&& any) {
  return get_proxy_ptr(get_proxy(std::forward<Any>(any)), get_v_table(any));
}

template <is_any Any>
  requires is_meta_data_v_table<typename Any::v_table_t>
inline const auto& get_meta_data(Any const& any) {
  return *get_v_table(any)->meta_data_;
}

template <is_any Any>
  requires is_type_info_v_table<typename Any::v_table_t>
inline std::type_info const& get_type_info(Any const& any) {
  return *get_v_table(any)->type_info_;
}

template <is_any Any>
  requires is_dynamic_castable_v_table<typename Any::v_table_t>
bool is_derived_from(const std::type_info& from, Any const& any) {
  return get_v_table(any)->is_derived_from_(from);
}
template <is_any From, is_any Any>
  requires is_dynamic_castable_v_table<typename From::v_table_t> &&
           is_dynamic_castable_v_table<typename Any::v_table_t>
bool is_derived_from(Any const& any) {
  return is_derived_from(typeid(typename From::v_table_t), any);
}

template <typename To>
  requires(!is_any<To>)
auto unchecked_v_table_downcast_to(observeable_v_table* v_table) {
  return static_cast<To*>(v_table);
}
template <typename To>
  requires is_any<To>
auto unchecked_v_table_downcast_to(auto* v_table) {
  return unchecked_v_table_downcast_to<typename To::v_table_t>(v_table);
}

template <typename Any>
  requires is_any<Any> && Any::is_dyn
inline auto get_v_table(Any const& any) {
  return unchecked_v_table_downcast_to<Any>(any.get_v_table_ptr());
}

template <is_any To, is_any From>
inline To unchecked_downcast_to(From from)
  requires(is_any_derived_from<To, From>)
{
  return To{std::move(from.proxy_),
            unchecked_v_table_downcast_to<To>(get_v_table(from))};
}

template <typename Trait, is_proxy Proxy>
inline auto release_v_table(any<Trait, Proxy>& from) {
  return from.release_v_table();
}

/// \defgroup casts Casts
/// \brief Mix and match downcast, crosscast, and obtain \ref any with other
///
/// IMPORTANT: For crosscasts to work, the models must be registered with the
/// \ref ANY_REGISTER_MODEL macro.

/// \brief Safe downcast to a derived trait using runtime information from the
/// v-Tables.
/// \ingroup casts
template <is_any To, is_any From>
inline std::optional<To> downcast_to(From from)
  requires(is_any_derived_from<To, From>)
{
  if (is_derived_from<To>(from))
    return {unchecked_downcast_to<To>(std::move(from))};
  return {};
}

template <typename U, is_any Any>
inline auto unchecked_unerase_cast(Any const& o) {
  return unchecked_unerase_cast<U>(get_proxy(o), get_v_table(o));
}
/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, typename Any>
  requires is_any<Any> && Any::is_dyn
inline auto unerase_cast(Any const& o) {
  return unerase_cast<U>(get_proxy(o), get_v_table(o));
}
/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, typename Any>
  requires is_any<Any> && Any::is_dyn
inline auto unerase_cast_if(Any const& o) {
  return unerase_cast_if<U>(get_proxy(o), get_v_table(o));
}

/// Proxy to capture the dispatch target concrete to enable static dispatch
/// A simple wrapper class over an object. Use \c '&' and \c 'const &' to
/// capture by reference.
///
/// Usage:
/// * Use the model map as a static customization point.
/// * Use \c using_<std::variant<...>> to unify customization points and member
///   function-like invocation.
/// * Use with \ref vany_variant.
///
/// \tparam Value The captured value
template <typename Value>
struct using_ {
  using_() = default;
  template <typename V>
    requires(!std::same_as<std::decay_t<V>, using_>)
  using_(V&& v) : value_(std::forward<V>(v)) {}
  Value value_;
  using value_t = Value;
  operator Value() const { return value_; }
  /// Helper type alias template to trait a model with an \ref any.
  /// See also \ref trait_as.
  template <typename Trait>
  using as = any<Trait, using_<Value>>;
};

/// Proxy to capture the dispatch target concrete via const& to enable static
/// dispatch A simple wrapper class over an object. Use \c '&' and \c 'const &'
/// to capture by reference.
///
/// Usage:
/// * Use the model map as a static customization point.
///
/// \tparam Value The captured value
template <typename Value>
struct using_cref {
  using_cref() = delete;
  template <typename V>
    requires(!std::same_as<std::decay_t<V>, using_cref>)
  using_cref(V const& v) : value_(v) {}
  Value const& value_;
  operator Value const&() const { return value_; }
  /// Helper type alias template to trait a model with an \ref any.
  /// See also \ref trait_as.
  template <typename Trait>
  using as = any<Trait, using_cref<Value>>;
};

/// A template to get an \ref any trait for a type
/// See also \ref using_::as.
template <typename Type, typename Trait>
using use_as = any<Trait, using_<Type>>;

/// A template to get an \ref any trait for a type
/// See also \ref using_cref::as.
template <typename Type, typename Trait>
using use_as_cref = any<Trait, using_cref<Type>>;

/// A template to get an templatetd \ref any trait for a type
/// See also \ref using_::as.
template <typename Type, template <typename...> typename Trait, typename... Ts>
using use_as_ = any<Trait<Ts...>, using_<Type>>;

/// A template to get an templatetd \ref any trait for a type
/// See also \ref using_cref::as.
template <typename Type, template <typename...> typename Trait, typename... Ts>
using use_as_cref_ = any<Trait<Ts...>, using_cref<Type>>;

/// A factory function to bind an object as a model to an \ref any with a \ref
/// trait.
/// See also \ref using_::as.
template <typename Trait, typename T>
auto trait_as(T&& v) {
  return any<Trait, using_<std::decay_t<T>>>{std::forward<T>(v)};
}

/// A factory function to bind a const& as a model to an \ref any with a \ref
/// trait.
/// See also \ref using_cref::as.
template <typename Trait, typename T>
auto trait_as_cref(T const& v) {
  return any<Trait, using_cref<std::decay_t<T>>>{v};
}

/// Proxy to capture the type to enable static
/// dispatch. Has no data member, so it doesn't capture any value and has no
/// size
///
/// \tparam Type The captured type
template <typename Type>
struct trait_class {
  using value_t = Type;
};

/// Helper type alias template to get a \ref trait_class for a type
/// dispatch. Has no data member, so it doesn't capture any value and has no
/// size
///
/// \tparam Type The captured type
template <typename Type, typename Trait>
using any_trait_class = any<Trait, trait_class<Type>>;

/// A object template to get a \ref trait_class object for a type as a \ref
/// trait.
/// See also \ref using_::as.
template <typename Type, typename Trait>
static inline any_trait_class<Type, Trait> trait_class_;

template <typename VTable, typename Concrete>
VTable* v_table_instance() {
  static VTable v_table{std::in_place_type<Concrete>};
  return &v_table;
}

using dispatch_table_function_t = void (*)();
using dispatch_table_dispatch_index_t = std::size_t;
using dispatch_table_entry_t = unsigned long long;
using dispatch_table_t = std::vector<dispatch_table_entry_t>;
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
#endif

#define ANY_OPEN_DISPATCH                                    \
  ANY_V_TABLE_DATA(anyxx::dispatch_table_t*, dispatch_table, \
                   anyxx::dispatch_table_instance<v_table_t, Concrete>())

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

template <typename VTable>
concept is_open_dispatch_v_table = requires(VTable* v_table) {
  { v_table->dispatch_table } -> std::convertible_to<dispatch_table_t*>;
};

template <is_any ToAny, is_dynamic_castable_v_table FromVTable>
  requires is_dynamic_castable_v_table<typename ToAny::v_table_t>
auto query_v_table(FromVTable* from)
    -> std::expected<typename ToAny::v_table_t*, anyxx::cast_error> {
  using v_table_t = typename ToAny::v_table_t;
  if (from->is_derived_from_(typeid(v_table_t)))
    return reinterpret_cast<v_table_t*>(from);
  if constexpr (is_meta_data_v_table<FromVTable>) {
    if (auto meta_data = from->meta_data_; meta_data) {
      return meta_data->template get_v_table<v_table_t>();
    } else {
      return std::unexpected(
          anyxx::cast_error{typeid(v_table_t), *from->type_info_});
    }
  } else {
    return std::unexpected(
        anyxx::cast_error{typeid(v_table_t), *from->type_info_});
  }
}

// --------------------------------------------------------------------------------
// any parameter translation

struct self {};

template <typename Param>
struct jacket_return;

template <typename Param>
  requires(!std::is_reference_v<Param>)
struct jacket_return<Param> {
  template <typename Sig>
  static Param forward(Sig&& sig, auto&&) {
    return std::forward<Sig>(sig);
  }
};
template <typename Param>
  requires std::is_reference_v<Param>
struct jacket_return<Param> {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig, auto&&) {
    return std::forward<Sig>(sig);
  }
};
static_assert(!is_type_class<val<>>);
template <>
struct jacket_return<self> {
  template <typename Sig, typename Any>
  static decltype(auto) forward(Sig&& sig, Any const&) {
    using sig_t = std::decay_t<Sig>;
    if constexpr (is_type_class<typename std::decay_t<Any>::proxy_t>) {
      using target_t =
          any<typename Any::trait_t, using_<typename Any::proxy_t::value_t>>;
      if constexpr (is_any<sig_t>) {
        return target_t{get_proxy_value(std::forward<Sig>(sig))};
      } else {
        return target_t{std::forward<Sig>(sig)};
      }
    } else {
      if constexpr (is_any<sig_t> && !Any::is_dyn) {
        return Any{get_proxy_value(sig)};
      } else {
        return Any{std::forward<Sig>(sig)};
      }
    }
  }
};
template <>
struct jacket_return<self&> {
  static auto& forward(auto, auto&& any) {
    return any;  // "return *this" semantics!
  }
};

template <typename T>
struct translate_sig_map {
  template <typename AnyValue>
  using v_table_param = T;
  template <typename AnyValue>
  using v_table_return = T;
  template <typename AnyValue>
  using map_return = T;
  template <typename AnyValue>
  using concept_arg = T;
};
template <>
struct translate_sig_map<self> {
  template <typename AnyValue>
  using v_table_param = any<dynamic_copyable, cref>;
  template <typename AnyValue>
  using v_table_return = AnyValue;
  template <typename Model>
  using map_return = Model;
  template <typename Model>
  using concept_arg = Model;
};
template <>
struct translate_sig_map<self const&> {
  template <typename AnyValue>
  using v_table_param = any<dynamic_copyable, cref>;
  template <typename AnyValue>
  using v_table_return = int;  // dummy
  template <typename Model>
  using map_return = Model const&;
  template <typename Model>
  using concept_arg = Model const&;
};
template <>
struct translate_sig_map<self&> {
  template <typename AnyValue>
  using v_table_param = any<dynamic_copyable, mutref>;
  template <typename AnyValue>
  using v_table_return = int;  // dummy
  template <typename Model>
  using map_return = Model&;
  template <typename Model>
  using concept_arg = Model&;
};
template <typename T>
struct is_use_as__impl : std::false_type {};
template <typename T, template <typename...> typename Trait,
          typename... TraitArgs>
struct is_use_as__impl<any<Trait<TraitArgs...>, using_<T>>> : std::true_type {};
template <typename T>
inline constexpr bool is_use_as_ = is_use_as__impl<std::decay_t<T>>::value;

template <typename T>
  requires is_use_as_<T>
struct translate_sig_map<T> {
  template <typename AnyValue>
  using v_table_param = any<dynamic_copyable, mutref>;
  template <typename AnyValue>
  using v_table_return = int;  // dummy
  template <typename Model>
  using map_return = T;
  template <typename Model>
  using concept_arg = typename std::decay_t<T>::rep_type;
};

template <typename AnyValue, typename Param>
using v_table_param =
    translate_sig_map<Param>::template v_table_param<AnyValue>;
template <typename AnyValue, typename Return>
using v_table_return =
    translate_sig_map<Return>::template v_table_return<AnyValue>;
template <typename Model, typename Param>
using map_return = translate_sig_map<Param>::template map_return<Model>;
template <typename Model, typename Param>
using concept_arg = translate_sig_map<Param>::template concept_arg<Model>;

//+++   This metafunctions cannot be expressed as traits, because they would
// be
//      recursive. So we need to use template specialization instead.
template <typename T>
struct handle_self_ref_return {
  static T operator()() {
    static std::remove_reference_t<T> dummy;
    return dummy;
  }
};
template <>
struct handle_self_ref_return<void> {
  static void operator()() {}
};
template <>
struct handle_self_ref_return<self&> {
  static int operator()() { return 0; }
};

template <typename Concrete, typename T>
struct v_table_to_map {
  template <typename Sig>
  static Sig&& forward(Sig&& sig) {
    return std::forward<Sig>(sig);
  }
};
template <typename Concrete>
struct v_table_to_map<Concrete, self&> {
  template <typename Sig>
  static Concrete& forward(Sig&& sig) {
    return *unerase_cast<Concrete>(sig);
  }
};
template <typename Concrete>
struct v_table_to_map<Concrete, self const&> {
  template <typename Sig>
  static Concrete const& forward(Sig&& sig) {
    return *unerase_cast<Concrete>(sig);
  }
};

template <typename, typename T>
struct forward_trait_to_map {
  template <typename Sig>
  static Sig&& forward(Sig&& sig) {
    return std::forward<Sig>(sig);
  }
};
template <typename Traited>
struct forward_trait_to_map<Traited, self&> {
  template <typename Sig>
  static Traited& forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
template <typename Traited>
struct forward_trait_to_map<Traited, self const&> {
  template <typename Sig>
  static Traited const& forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
template <typename Traited, typename T>
  requires is_use_as_<T>
struct forward_trait_to_map<Traited, T const&> {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
//---

// --------------------------------------------------------------------------------
// any customization traits

template <is_proxy Proxy = val<>>
struct default_proxy {
  using type = Proxy;
};

// --------------------------------------------------------------------------------
// typed any

template <typename V, is_any Any>
struct typed_any : public Any {
  using any_t = Any;
  using proxy_t = typename any_t::proxy_t;
  using proxy_trait_t = any_t::proxy_trait_t;
  using void_t = proxy_trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using any_t::any_t;

  // cppcheck-suppress-begin noExplicitConstructor
  explicit(false) typed_any(V const& v) : any_t(v) {}        // NOLINT
  explicit(false) typed_any(V&& v) : any_t(std::move(v)) {}  // NOLINT
  explicit(false) typed_any(any_t i) : any_t(i) {            // NOLINT
    check_type_match<V>(get_v_table(*this));
  }
  // cppcheck-suppress-end noExplicitConstructor

  value_t const& operator*() const {
    return *unchecked_unerase_cast<value_t const>(*this);
  }
  value_t const* operator->() const {
    return unchecked_unerase_cast<value_t const>(*this);
  }
  value_t const* get() const {
    return unchecked_unerase_cast<value_t const>(*this);
  }
  value_t& operator*() const
    requires(!is_const)
  {
    return *unchecked_unerase_cast<value_t>(*this);
  }
  value_t* operator->() const
    requires(!is_const)
  {
    return unchecked_unerase_cast<value_t>(*this);
  }
  value_t* get() const
    requires(!is_const)
  {
    return unchecked_unerase_cast<value_t>(*this);
  }
  explicit operator bool() const { return static_cast<bool>(this->proxy_); }
};

template <typename V, is_any Any>
auto as(Any source) {
  return typed_any<V, Any>{std::move(source)};
}

template <typename To, typename V, is_any Any>
auto as(typed_any<V, Any> source)
  requires std::convertible_to<V*, To*>
{
  if constexpr (typed_any<V, Any>::is_const) {
    return typed_any<To const, Any>{std::move(source.proxy_)};
  } else {
    return typed_any<To, Any>{std::move(source.proxy_)};
  }
}

// --------------------------------------------------------------------------------
// any borrow, clone, lock, move

template <is_any ToAny, is_proxy FromProxy, typename FromVTable>
  requires proxy_borrowable_from<typename ToAny::proxy_t, FromProxy, FromVTable>
std::expected<ToAny, cast_error> borrow_as(FromProxy const& from,
                                           FromVTable* from_v_table) {
  using to = typename ToAny::proxy_t;
  return query_v_table<ToAny>(from_v_table).transform([&](auto v_table) {
    return ToAny{borrow_proxy_as<to>(from, v_table), v_table};
  });
}

/// \brief Safe crosscast via runtime information to another \ref any without
/// changing the ownership.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
  requires proxy_borrowable_from<typename ToAny::proxy_t,
                                 typename FromAny::proxy_t,
                                 typename FromAny::v_table_t>
std::expected<ToAny, cast_error> borrow_as(FromAny const& from) {
  if constexpr (is_any_derived_from<FromAny, ToAny>) {
    return {ToAny{from}};
  } else if constexpr (is_any_derived_from<ToAny, FromAny>) {
    return *downcast_to<ToAny>(from);
  } else {
    return borrow_as<ToAny>(get_proxy(from), get_v_table(from));
  }
}

/// \brief Clone via runtime information.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
std::expected<ToAny, cast_error> clone_to(FromAny const& from) {
  using vv_to_t = typename ToAny::proxy_t;
  static_assert(is_proxy<vv_to_t>);
  return query_v_table<ToAny>(get_v_table(from)).transform([&](auto v_table) {
    return ToAny{clone_to<vv_to_t>(get_proxy(from), v_table), v_table};
  });
}

/// \brief Lock a shared \ref any.
/// \ingroup casts
template <is_any FromAny>
  requires std::same_as<typename FromAny::proxy_t, weak>
auto lock(FromAny const& from_interface) {
  using to_any_t = any<typename FromAny::trait_t, shared>;
  static_assert(is_any<to_any_t>);
  using return_t = std::optional<to_any_t>;
  if (auto locked = get_proxy(from_interface).lock())
    return return_t{to_any_t{std::move(locked), get_v_table(from_interface)}};
  return return_t{};
}

/// \brief Move ownership to another \ref any. Uses runtime information to
/// crosscast, if necessary.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
ToAny move_to(FromAny&& from) {
  auto to_v_table = query_v_table<ToAny>(release_v_table(from));
  return ToAny{move_proxy(std::move(from)), *to_v_table};
}

template <typename Concrete>
mutable_void invoke_move_constructor([[maybe_unused]] mutable_void placement,
                                     [[maybe_unused]] mutable_void from) {
  static_assert(std::is_move_constructible_v<Concrete>);
  auto typed_placement = static_cast<Concrete*>(placement);
  auto typed_from = static_cast<Concrete*>(from);
  [[maybe_unused]] auto constructed =
      std::construct_at<Concrete>(typed_placement, std::move(*typed_from));
  assert(placement == constructed);
  std::destroy_at(typed_from);
  return placement;
}

template <typename Concrete>
  requires std::copy_constructible<Concrete>
mutable_void invoke_copy_constructor([[maybe_unused]] mutable_void placement,
                                     [[maybe_unused]] const_void from) {
  return std::construct_at<Concrete>(static_cast<Concrete*>(placement),
                                     *static_cast<Concrete const*>(from));
}

#define ANY_CLASS_TYPE_INFO \
  ANY_V_TABLE_DATA(std::type_info const*, type_info_, &typeid(Concrete))
#define ANY_CAN_TYPE_SAVE_DOWNCAST                                           \
  ANY_V_TABLE_DATA(                                                          \
      is_derived_from_t, is_derived_from_, +[](const std::type_info& from) { \
        return static_is_derived_from(from);                                 \
      })
#define ANY_CAN_TYPE_SAVE_CROSSCAST \
  ANY_V_TABLE_DATA(meta_data*, meta_data_, nullptr)
#define ANY_MODEL_SIZE \
  ANY_V_TABLE_DATA(model_size_t, model_size, compute_model_size<Concrete>())
#define ANY_COPY_CONSTRUCTOR                                              \
  ANY_V_TABLE_DATA(copy_constructor_t, copy_constructor,                  \
                   []([[maybe_unused]] mutable_void placement,            \
                      [[maybe_unused]] const_void from) -> mutable_void { \
                     return invoke_copy_constructor<Concrete>(placement,  \
                                                              from);      \
                   })
#define ANY_HAS_DELETE                                        \
  ANY_V_TABLE_DATA(delete_t, delete_, [](mutable_void data) { \
    if (data) delete static_cast<Concrete*>(data);            \
  })
#define ANY_MOVE_CONSTRUCTOR                                                \
  ANY_V_TABLE_DATA(move_constructor_t, move_constructor,                    \
                   []([[maybe_unused]] mutable_void placement,              \
                      [[maybe_unused]] mutable_void from) -> mutable_void { \
                     return invoke_move_constructor<Concrete>(placement,    \
                                                              from);        \
                   })
#define ANY_DESTRUCTOR                                               \
  ANY_V_TABLE_DATA(destructor_t, destructor, [](mutable_void data) { \
    std::destroy_at(static_cast<Concrete*>(data));                   \
  })

TRAIT_EX_(const_referenceable, observeable, , , , ,
          (using default_proxy_t = cref;));
TRAIT_EX_(mutable_referenceable, observeable, , , , ,
          (using default_proxy_t = mutref;));

TRAIT_EX_(moveable, observeable, , , ,
          (ANY_MODEL_SIZE, ANY_MOVE_CONSTRUCTOR, ANY_DESTRUCTOR),
          (using default_proxy_t = val<>;));

TRAIT_EX_(copyable, moveable, , , , (ANY_COPY_CONSTRUCTOR), ());

TRAIT_EX_(save_observable, observeable, , , , (ANY_CLASS_TYPE_INFO), ());

TRAIT_EX_(save_moveable, save_observable, , , ,
          (ANY_MODEL_SIZE, ANY_MOVE_CONSTRUCTOR, ANY_DESTRUCTOR),
          (using default_proxy_t = val<>;));

TRAIT_EX_(save_copyable, save_moveable, , , , (ANY_COPY_CONSTRUCTOR), ());

TRAIT_EX_(dynamic_castable, save_observable, , , ,
          (ANY_CAN_TYPE_SAVE_DOWNCAST, ANY_CAN_TYPE_SAVE_CROSSCAST), ());

TRAIT_EX_(dynamic_deletable, dynamic_castable, , , , (ANY_HAS_DELETE),
          (using default_proxy_t = shared;));

TRAIT_EX_(dynamic_smart_ptr, dynamic_deletable, , , ,
          (ANY_MODEL_SIZE, ANY_COPY_CONSTRUCTOR), ());

TRAIT_EX_(dynamic_moveable, dynamic_castable, , , ,
          (ANY_MODEL_SIZE, ANY_MOVE_CONSTRUCTOR, ANY_DESTRUCTOR),
          (using default_proxy_t = val<>;));

TRAIT_EX_(dynamic_copyable, dynamic_moveable, , , , (ANY_COPY_CONSTRUCTOR), ());

class meta_data {
  const std::type_info& type_info_;

  struct i_table_entry {
    void* v_table_ = nullptr;
    std::type_index type_index_;

    template <typename VTable>
    i_table_entry(VTable* v_table)
        : v_table_(v_table), type_index_(typeid(*v_table)) {}
  };

  std::vector<i_table_entry> i_table_;

 public:
  template <typename CLASS>
  explicit constexpr meta_data(std::in_place_type_t<CLASS>)
      : type_info_(typeid(CLASS)) {}

  constexpr const std::type_info& get_type_info() const { return type_info_; }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  template <typename VTable>
  VTable* find_v_table() const {
    if (auto found =
            std::ranges::find(get_i_table(), std::type_index(typeid(VTable)),
                              &i_table_entry::type_index_);
        found != get_i_table().end())
      return static_cast<VTable*>(found->v_table_);
    return nullptr;
  }

  template <typename VTable>
  std::expected<VTable*, cast_error> get_v_table() const {
    if (auto v_table = find_v_table<VTable>(); v_table) return v_table;
    return std::unexpected(
        cast_error{.to = typeid(VTable), .from = get_type_info()});
  }
  template <typename VTable>
  auto register_v_table(VTable* v_table) {
    v_table->meta_data_ = this;
    if (!find_v_table<VTable>()) i_table_.push_back({v_table});
    return v_table;
  }
};

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data_{std::in_place_type<TYPE>};
  return meta_data_;
}

// --------------------------------------------------------------------------------
// hook

/// \brief A class template to implement a hook/callback chain.
template <typename R, typename... Args>
class hook;
template <typename R, typename... Args>
class hook<R(Args...)> {
 public:
  struct connection_info {
    int id;
    hook* owner;
  };
  class connection {
    connection_info info_;
    friend class hook;

    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

   public:
    // cppcheck-suppress-begin noExplicitConstructor
    explicit(false) connection(connection_info info) : info_(info) {}
    // cppcheck-suppress-end noExplicitConstructor
    connection& operator=(connection_info info) {
      close();
      info_ = info;
      return *this;
    }
    connection(connection&&) = default;
    connection& operator=(connection&&) = default;
    void close() {
      if (info_.owner) info_.owner->remove(info_.id);
      info_.owner = nullptr;
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
      return hook_.callees_[((std::size_t)index_)].second(
          super{index_ - 1, hook_}, std::forward<Args>(args)...);
    }
  };

  using callee = std::function<R(super const&, Args...)>;

  R operator()(Args&&... args) const {
    assert(!callees_.empty());
    return callees_.back().second(super{((int)callees_.size()) - 2, *this},
                                  std::forward<Args>(args)...);
  }

  connection_info insert(callee const& f) {
    callees_.emplace_back(entry{next_id_, f});
    return connection_info{next_id_++, this};
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
/// \brief A key type for factory registration.
template <typename Tag>
struct key {
  const char* label;
  friend auto operator<=>(key, key) = default;
};
template <typename T>
struct is_key_impl : std::false_type {};
template <typename T>
struct is_key_impl<key<T>> : std::true_type {};
template <typename T>
concept is_key = is_key_impl<T>::value;

/// \brief A class template to implement a factory for \ref any
/// objects.
template <typename Any, typename Key, typename... Args>
  requires proxy_trait<typename Any::proxy_t>::is_owner
class factory {
  using constructor_t = std::function<Any(Args...)>;
  std::map<Key, constructor_t> factory_map_;

  auto register_impl(Key key, auto const& construct) {
    factory_map_[key] = [construct](Args... args) -> Any {
      return Any{std::in_place, construct(std::forward<Args>(args)...)};
    };
  }

  auto construct_impl(Key key, Args... args) {
    if (auto found = factory_map_.find(key); found != factory_map_.end())
      return found->second(std::forward<Args>(args)...);
    if constexpr (std::same_as<Key, std::string>) {
      throw unkonwn_factory_key_error{key};
    } else if constexpr (is_key<Key>) {
      throw unkonwn_factory_key_error{key.label};
    } else {
      throw unkonwn_factory_key_error{std::to_string(key)};
    }
  };

 public:
  auto register_(Key const& key, auto const& construct) {
    register_impl(key, construct);
    return nullptr;
  }
  auto construct(auto key, Args&&... args) {
    return construct_impl(key, std::forward<Args>(args)...);
  }
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

/// \brief A class template to implement extension members.
template <typename InObject>
struct members {
  members() : table_(members_count<InObject>()) {}
  using any_value_t = any<dynamic_copyable, nullable_val>;
  std::vector<any_value_t> table_;
  template <typename Member, typename Arg>
  void set(Member member, Arg&& arg) {
    using value_t = typename Member::value_t;
    table_[member.index] =
        any_value_t{std::in_place_type<value_t>, std::forward<Arg>(arg)};
  }
  template <typename Member>
  typename Member::value_t const* get(Member member) const {
    const auto& val = table_[member.index];
    if (!val) return {};
    return unchecked_unerase_cast<typename Member::value_t>(val);
  }
  template <typename Member>
  typename Member::value_t* get(Member member) {
    auto& val = table_[member.index];
    if (!val) return {};
    return unchecked_unerase_cast<typename Member::value_t>(val);
  }
  template <typename Member>
  typename Member::value_t& operator[](Member member) {
    if (auto val = get(member)) {
      return *val;
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
/// \brief A counter for dispatch tables per v-table type.
template <typename AnyVTable>
std::size_t& dispatchs_count() {
  static std::size_t count = 0;
  return count;
}
#endif

/// \brief A tag to indicate that the corresponding function
/// parameter is an
/// \ref any used for dispatch.
///
/// \tparam Any The \ref any used for dispatch
template <is_any Any>
  requires(Any::is_dyn && is_open_dispatch_v_table<typename Any::v_table_t>)
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
  static auto instance(auto functor)  // if functor is a templated operator()
                                      // from a stateless function object,
                                      // instantiate it now!;
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
                       std::make_tuple(get_proxy_ptr(dispatch_arg))),
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

class no_default_function_error : public error {
  using error::error;
};

template <typename R, typename... OuterArgs>
struct dispatch_function_types {
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
              if constexpr (std::is_default_constructible_v<R>) {
                return R{};
              } else {
                throw no_default_function_error("no default function");
              }
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

  using default_type = outer<OuterArgs...>::template type<>;
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

/// \brief Open dispatch method. Solves the expression problem. See
/// \ref dispatch_sig "dispatch<R(Args...)>" for details.
///
/// \tparam R Return type.
/// \tparam ...Args Parameter types. The parameters for open dispatch
/// must be the first and braced as \ref virtual_.
template <typename R, typename... Args>
class dispatch;
/// \anchor dispatch_sig
/// \brief Open dispatch method. Solves the expression problem.
/// \tparam R The return type.
/// \tparam Args The parameter types. The signature of the dispatch
/// is R(Args...).
///
/// The open dispatch is managed via a singleton object of this
/// class.
///
/// To declare and define a singleton, use the \ref ANY_SINGLETON and
/// \ref ANY_SINGLETON_DECLARE macros.
///
/// Each trait of the \ref any tagged as virtual must have open
/// dispatch enabled. To enable open dispatch, declare a struct named
/// 'trait_name'_has_open_dispatch.
/// \code
/// struct node_has_open_dispatch {};
/// ANY(node<>, , )
/// // open dispatch evaluate, returns int, dispatched via an
/// any_node; dispatch<int(virtual_<any_node<>>)> evaluate;
/// \endcode
/// Examples are listed here: \ref dispatch
template <typename R, typename... Args>
class dispatch<R(Args...)> {
 public:
  using erased_function_t =
      typename translate_erased_function<R, Args...>::type;

  static constexpr std::size_t dimension_count =
      dispatch_dimension_count<0, Args...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, Args...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using default_ = typename dispatch_function_types<R, Args...>::default_type;
  default_::function_t dispatch_default_hook_;
  default_::function_t::connection default_connection_ =
      dispatch_default_hook_.insert(default_::function());

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

      if (target.size())
        return next_t::invoke(
            target[0],
            std::tuple_cat(get_tuple_head<Dimension>(dispatch_args_tuple),
                           std::make_tuple(&any),
                           get_tuple_tail<Dimension + 1>(dispatch_args_tuple)),
            std::forward<ActualArgs>(actual_args)...);
      return {};
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
    R invoke(default_::function_t const& default_, Any const& any,
             Other&&... other) const {
      auto v_table = get_v_table(any)->dispatch_table;
      auto target = get_function(v_table, index_);
      if (!target)
        return std::invoke(default_, any, std::forward<Other>(other)...);
      auto erased_function = reinterpret_cast<erased_function_t>(target);
      return std::invoke(erased_function, get_proxy_ptr(any),
                         std::forward<Other>(other)...);
    }
  };

  static const constexpr kind dispatch_kind =
      (dimension_count > 1) ? kind::multiple : kind::single;
  dispatch_access<dispatch_kind, 0, Args...> dispatch_access_;

 public:
  /// \brief Register a dispatch target for the model provided in the
  /// type parameter.
  /// \tparam Classes The models for which the dispatch target is
  /// registered.
  /// \param f The dispatch target.
  /// \return Dummy value, useful for initializing an auto unnamed
  /// variable at (static) namespace scope.
  template <typename... Classes>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, Classes...>::template instance<Args...>(f);
    return dispatch_access_.template define<Classes...>(fp, dispatch_matrix_);
  };
  /// \brief Invoke the dispatch.
  /// \tparam ActualArgs The deduced argument types for the dispatch
  /// call.
  /// \param actual_args Arguments fulfilling the signature of this
  /// dispatch.
  /// \return The result of the dispatched function call.
  ///
  /// The called function is determined by the \ref any objects
  /// marked with \ref virtual_ in the class instantiation.
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

template <size_t At, typename Tuple, size_t... Is>
auto make_tuple_from_elements(Tuple&& tuple, std::index_sequence<Is...>) {
  return std::forward_as_tuple(
      std::get<Is + At>(std::forward<Tuple>(tuple))...);
}
template <size_t At, size_t N, typename Tuple>
auto make_tuple_from_elements_at(Tuple&& tuple) {
  return make_tuple_from_elements<At>(std::forward<Tuple>(tuple),
                                      std::make_index_sequence<N>{});
}

template <typename Vany, typename DynamicDispatch, auto StaticDispatch>
class dispatch_vany {
  DynamicDispatch dynamic_dispatch_;
  constexpr static const std::size_t dimension_count =
      DynamicDispatch::dimension_count;

  template <typename Vany1, typename... Args>
  auto invoke1(Vany1&& vany, Args&&... args) const {
    return std::visit(
        [&]<typename TypedArg>([[maybe_unused]] TypedArg&& arg) {
          return overloads{
              StaticDispatch,
              [&]<is_any Any, typename... Vargs>(Any&& any, Vargs&&... vargs) {
                return dynamic_dispatch_(std::forward<Any>(any),
                                         std::forward<Vargs>(vargs)...);
              }}(std::forward<TypedArg>(arg), std::forward<Args>(args)...);
        },
        get_proxy_value(std::forward<Vany1>(vany)));
  }

  template <is_any Vany1, is_any Vany2, typename... Args>
  auto invoke2(Vany1&& vany1, Vany2&& vany2, Args&&... args) const {
    using vany1_t = std::decay_t<Vany1>;
    using vany2_t = std::decay_t<Vany2>;
    using cv1_t = anyxx::vany_type_trait<vany1_t>::concrete_variant;
    using any_v1 = anyxx::vany_type_trait<vany1_t>::any_in_variant;
    using cv2_t = anyxx::vany_type_trait<vany2_t>::concrete_variant;
    using any_v2 = anyxx::vany_type_trait<vany2_t>::any_in_variant;

    auto dispatch_combined = [&]<typename DA1, typename DA2>(DA1&& da1,
                                                             DA2&& da2) {
      auto dyn_case1 = [&]<is_any A1, is_any A2, typename... VAs>(
                           A1&& a1, A2&& a2, VAs&&... vas) {
        return dynamic_dispatch_(std::forward<A1>(a1), std::forward<A2>(a2),
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case2 = [&]<is_any A1, typename A2, typename... VAs>(
                           A1&& a1, A2 a2, VAs&&... vas)
        requires std::constructible_from<cv2_t, A2>
      {
        return dynamic_dispatch_(std::forward<A1>(a1),
                                 any_v2{std::in_place, cv2_t{std::move(a2)}},
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case3 = [&]<typename A1, is_any A2, typename... VAs>(
                           A1 a1, A2&& a2, VAs&&... vas)
        requires std::constructible_from<cv1_t, A1>
      {
        return dynamic_dispatch_(any_v1{std::in_place, cv1_t{std::move(a1)}},
                                 std::forward<A2>(a2),
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case4 = [&]<typename A1, typename A2, typename... VAs>(
                           A1 a1, A2 a2, VAs&&... vas)
        requires(std::constructible_from<cv1_t, A1> &&
                 std::constructible_from<cv2_t, A2>)
      {
        return dynamic_dispatch_(any_v1{std::in_place, cv1_t{std::move(a1)}},
                                 any_v2{std::in_place, cv2_t{std::move(a2)}},
                                 std::forward<VAs>(vas)...);
      };
      return overloads{StaticDispatch, dyn_case1, dyn_case2, dyn_case3,
                       dyn_case4}(std::forward<DA1>(da1),
                                  std::forward<DA2>(da2),
                                  std::forward<Args>(args)...);
    };

    return std::visit(dispatch_combined,
                      get_proxy_value(std::forward<Vany1>(vany1)),
                      get_proxy_value(std::forward<Vany2>(vany2)));
  }

 public:
  template <typename... Classes>
  auto define(auto f) {
    return dynamic_dispatch_.template define<Classes...>(f);
  }

  template <typename... Args>
  auto operator()(Args&&... args) const {
    if constexpr (dimension_count == 1) {
      return invoke1(std::forward<Args>(args)...);
    } else {
      if constexpr (dimension_count == 2) {
        return invoke2(std::forward<Args>(args)...);
      } else {
        static_assert(dimension_count <= 2,
                      "dispatch_vany only supports one and two dimensions");
      }
    }
  }
};
}  // namespace anyxx

#define ANY_MERGE_(a, b) a##b
#define ANY_LABEL_(a) ANY_MERGE_(unique_name_, a)
#define ANY_UNIQUE_NAME_ ANY_LABEL_(__COUNTER__)
#define ANY_UNIQUE_NAME ANY_UNIQUE_NAME_
#define __ ANY_UNIQUE_NAME_

/// \addtogroup singleton_macros ANY_SINGLETON macros
/// \brief Macros to reduce boilerplate for declaring/defining static
/// runtime data.
///
/// To enable some dynamic functionality (crosscast, open dispatch)
/// there must be some static data holding information to perform
/// these operations. Extra care must be taken in a DLL scenario.
///
/// To reduce boilerplate code, Any++ supplies these macros.
///  @{

/// \brief Declare access to a singleton object in a header file.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param name Name of the singleton.
/// \param __VA_ARGS__ Type of the singleton object.
///
/// This macro should reside inside the namespace you wish the
/// singleton to reside. See also \ref ANY_SINGLETON.
#define ANY_SINGLETON_DECLARE(export_, name, ...) \
  using name##_t = __VA_ARGS__;                   \
  export_ extern name##_t& get_##name();          \
  static inline name##_t& name = get_##name();

/// \brief Define a singleton object in a source file.
/// \param namespace_ The namespace for the singleton.
/// \param name Name of the singleton.
/// \param __VA_ARGS__ Parameters for the singleton constructor.
///
/// See also \ref ANY_SINGLETON_DECLARE.
#define ANY_SINGLETON(namespace_, name, ...)       \
  namespace_::name##_t& namespace_::get_##name() { \
    static name##_t singleton_{__VA_ARGS__};       \
    return singleton_;                             \
  };
///  @}

/// \addtogroup vany_macros VANY_DISPACH macros
/// \brief Macros to reduce boilerplate for declaring/defining open
/// dispatch with \ref vany_variant. Only neccessary for DLL
/// scenarios
///  @{

/// \brief Declare a singleton object for open \ref vany dispatch.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param name Name of the dispatch.
/// \param vany Type of \ref vany.
/// \param signature Signature of the \ref dispatch.
/// \param static_dispatch Static constexpr visitor for the non-any
/// members of
///        the \ref vany.
///
/// See also \ref VANY_DISPACH, \ref ANY_SINGLETON_DECLARE.
#define VANY_DISPACH_DECLARE(export_, name, vany, signature, static_dispatch) \
  constexpr static inline auto name##_static_dispatch =                       \
      anyxx::overloads{_detail_REMOVE_PARENS(static_dispatch)};               \
                                                                              \
  using name##_vany = vany;                                                   \
  using name##_dynamic_dispatch =                                             \
      anyxx::dispatch<_detail_REMOVE_PARENS(signature)>;                      \
                                                                              \
  ANY_SINGLETON_DECLARE(                                                      \
      , name,                                                                 \
      anyxx::dispatch_vany<name##_vany, name##_dynamic_dispatch,              \
                           name##_static_dispatch>)

/// \brief Define a \ref vany dispatch singleton object in a source
/// file.
/// \param namespace_ The namespace for the singleton.
/// \param name Name of the singleton.
///
/// See also \ref VANY_DISPACH_DECLARE, \ref ANY_SINGLETON.
#define VANY_DISPACH(namespace_, name) ANY_SINGLETON(namespace_, name);
///  @}

#ifdef ANY_DLL_MODE

#define ANY_META_CLASS_FWD(export_, ...) \
  template <>                            \
  export_ anyxx::meta_data& anyxx::get_meta_data<__VA_ARGS__>();

#define ANY_META_CLASS(...)                                             \
  template <>                                                           \
  anyxx::meta_data& anyxx::get_meta_data<std::decay_t<__VA_ARGS__>>() { \
    return runtime_implementation<__VA_ARGS__>();                       \
  }

#else

/// \addtogroup meta_class_macros ANY_META_CLASS macros for crosscast
/// \brief Macros to define static runtime type data for a \ref
/// model. Only neccessary for DLL \b and crosscast scenarios.
/// @{

/// \def ANY_META_CLASS_FWD
/// \brief Declare access to the meta data for a specific model any.
/// Must be in global namespace.
/// \param export_ To supply an export macro in a DLL scenario
/// \param ... Type of the model
#define ANY_META_CLASS_FWD(...)
/// \def ANY_META_CLASS
/// \brief Define the meta data for a specific model any. Must be in
/// global namespace.
/// \param ... Type of the model
#define ANY_META_CLASS(...)

#endif

#define ANY_META_CLASS_STATIC(...)  \
  ANY_META_CLASS_FWD(, __VA_ARGS__) \
  ANY_META_CLASS(__VA_ARGS__)

/// \def ANY_REGISTER_MODEL
/// \brief Register a model class for a specific any interface. Must
/// be in global namespace.
/// \param class_ The model class with fully qualified name. Must be
/// parenthesized
/// \param interface_ Name of the \ref any (without any_ prefix).
/// \param ... Optional template parameters for the model class.
///
/// See also \ref casts.
#define ANY_REGISTER_MODEL(class_, interface_, ...)                           \
  namespace {                                                                 \
  static auto __ = anyxx::bind_v_table_to_meta_data<                          \
      interface_##_v_table _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(__VA_ARGS__), \
      ANYXX_UNPAREN(class_)>();                                               \
  }

/// @}

#ifdef ANY_DLL_MODE

#define ANY_MEMBERS_COUNT_FWD(export_, ns_, c_)  \
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

#define ANY_DISPATCH_COUNT_FWD(export_, ns_, any_)             \
  namespace ns_ {}                                             \
  namespace anyxx {                                            \
  template <>                                                  \
  export_ std::size_t& dispatchs_count<ns_::any_##_v_table>(); \
  }

#define ANY_DISPATCH_COUNT(ns_, any_)                          \
  template <>                                                  \
  std::size_t& anyxx::dispatchs_count<ns_::any_##_v_table>() { \
    static std::size_t count = 0;                              \
    return count;                                              \
  }

#define ANY_DISPATCH_FOR_FWD(export_, class_, interface_namespace_, \
                             interface_)                            \
  namespace anyxx {                                                 \
  template <>                                                       \
  export_ dispatch_table_t* dispatch_table_instance<                \
      interface_namespace_::interface_##_v_table, class_>();        \
  }

#define ANY_DISPATCH_FOR(class_, interface_namespace_, interface_) \
  template <>                                                      \
  anyxx::dispatch_table_t* anyxx::dispatch_table_instance<         \
      interface_namespace_::interface_##_v_table, class_>() {      \
    return dispatch_table_instance_implementation<                 \
        interface_namespace_::interface_##_v_table, class_>();     \
  }

#else

/// \addtogroup dispatch_macros DISPATCH_ macros
/// \brief Macros to define static runtime data for open dispatch.
/// Only neccessary for DLL scenarios.
///
/// Name conventions:
/// _FWD: macro to declare the function signature only. To be used in
/// header files.
///
/// ANY_DISPATCH_COUNT macros declare/define the dispatch counter for
/// a specific any. This dispatch counter is used to assign unique
/// indices to each dispatch.
///
/// ANY_DISPATCH_FOR macros declare/define the dispatch table
/// instance for a any. This is necessary once for each model class
/// that participates in open dispatch.
///
///  @{

/// \def ANY_DISPATCH_COUNT_FWD
/// \brief Declare access to the dispatch counter for a specific \ref
/// any. Must be placed in global namespace.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param ns_ Namespace of the \ref any.
/// \param any_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_COUNT_FWD(...)
/// \def ANY_DISPATCH_COUNT
/// \brief Define the dispatch counter for a specific \ref any. Must
/// be placed in global namespace.
/// \param ns_ Namespace of the \ref any.
/// \param any_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_COUNT(...)
/// \def ANY_DISPATCH_FOR_FWD
/// \brief Declare access to the dispatch table instance function for
/// a model. Must be placed in global namespace.
/// \param export_ To supply an export macro in a DLL scenario.
/// \param class_ The model class with fully qualified name.
/// \param interface_namespace_ Namespace of the \ref any.
/// \param interface_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_FOR_FWD(...)
/// \def ANY_DISPATCH_FOR
/// \brief Define the dispatch table instance function for a model.
/// Must be placed in global namespace
/// \param class_ The model class with fully qualified name.
/// \param interface_namespace_ Namespace of the \ref any.
/// \param interface_ Name of the \ref any (without any_ prefix).
#define ANY_DISPATCH_FOR(...)

///  @}

#endif

/// \defgroup anyxx_config Any++ configuration macro
/// \brief Macro to configure Any++ for DLL mode
///
/// If ANY_DLL_MODE is #defined, Any++ is configured for DLL mode. In
/// DLL mode, some static runtime data is not instantiated in the
/// header implicitly via static inline and must be manually
/// instantiated in a single translation unit.
///
/// See also \ref ANY_SINGLETON_DECLARE, \ref ANY_SINGLETON, \ref
/// ANY_META_CLASS_FWD, \ref ANY_META_CLASS, \ref
/// ANY_DISPATCH_COUNT_FWD, \ref ANY_DISPATCH_COUNT, \ref
/// ANY_DISPATCH_FOR_FWD, \ref ANY_DISPATCH_FOR, \ref
/// ANY_META_CLASS_FWD, \ref ANY_META_CLASS.

/**
   \example _1_any_shape.cpp

   \example _2d_trait_self.cpp
   Shows self a referntial trait
   \example _2f_trait_partial_equality.cpp
   Trait for partial equality. Walkthrough for advanced trait usage:
   - Static and dynamic polymorphism
   - the model map as a customization points
   - apply the automatic supplied C++20 concepts


   \example _2b_trait_monoid.cpp
   A self referntial trait that models a variant of a monoid.
   Show how to use a MODEL_MAP that acts simultanious as runtime and
   complitem customization point.

   \example _2f_trait_equal_comparable.cpp
   A step by step walkthrough for implementing a trait that models
   equality comparability for various types.

   \example _2e_trait_algebra.cpp
   A hierarchy of traits for the algebraic structures semigroup,
   monoid and group.

   \example _2p_trait_optional.cpp
   An optional as a customizable trait.

   \example _2c_trait_any_variant.cpp

   \example _2o_trait_simple.cpp
   Simple trait usage

   \example _3_any_range.cpp
   Type erased range example
   \example _5_any_template.cpp
   Templated anys recursively used
   \example 21_Tree_any.cpp
   Any in a tree structure
   \example 21_Tree_any_borrow_as.cpp
   factory, serialization and crosscast example
   \example 21_Tree_any_dispatch.cpp
   Open dispatch example
   \example 31_Animals_any_dispatch.cpp
   Open multi-dispatch example
   \example X1_any_weak_const.cpp
   Using any with weak and const proxies
   \example README.cpp
   README showcases
*/

// self tests...
namespace anyxx {
static_assert(!proxy_borrowable_from<mutref, cref, observeable_v_table>);
static_assert(proxy_borrowable_from<mutref, mutref, observeable_v_table>);
static_assert(proxy_borrowable_from<mutref, unique, observeable_v_table>);
static_assert(!proxy_borrowable_from<mutref, shared, observeable_v_table>);
static_assert(!proxy_borrowable_from<mutref, weak, observeable_v_table>);
static_assert(proxy_borrowable_from<mutref, val<>, dynamic_copyable_v_table>);

static_assert(proxy_borrowable_from<cref, cref, observeable_v_table>);
static_assert(proxy_borrowable_from<cref, mutref, observeable_v_table>);
static_assert(proxy_borrowable_from<cref, unique, observeable_v_table>);
static_assert(proxy_borrowable_from<cref, shared, observeable_v_table>);
static_assert(!proxy_borrowable_from<cref, weak, observeable_v_table>);
static_assert(proxy_borrowable_from<cref, val<>, dynamic_copyable_v_table>);

static_assert(!proxy_borrowable_from<shared, cref, observeable_v_table>);
static_assert(!proxy_borrowable_from<shared, mutref, observeable_v_table>);
static_assert(!proxy_borrowable_from<shared, unique, observeable_v_table>);
static_assert(proxy_borrowable_from<shared, shared, observeable_v_table>);
static_assert(!proxy_borrowable_from<shared, weak, observeable_v_table>);
static_assert(!proxy_borrowable_from<shared, val<>, dynamic_copyable_v_table>);

static_assert(!proxy_borrowable_from<weak, cref, observeable_v_table>);
static_assert(!proxy_borrowable_from<weak, mutref, observeable_v_table>);
static_assert(!proxy_borrowable_from<weak, unique, observeable_v_table>);
static_assert(proxy_borrowable_from<weak, shared, observeable_v_table>);
static_assert(proxy_borrowable_from<weak, weak, observeable_v_table>);
static_assert(!proxy_borrowable_from<weak, val<>, dynamic_copyable_v_table>);

static_assert(!proxy_borrowable_from<unique, cref, observeable_v_table>);
static_assert(!proxy_borrowable_from<unique, mutref, observeable_v_table>);
static_assert(!proxy_borrowable_from<unique, unique, observeable_v_table>);
static_assert(!proxy_borrowable_from<unique, shared, observeable_v_table>);
static_assert(!proxy_borrowable_from<unique, weak, observeable_v_table>);
static_assert(!proxy_borrowable_from<unique, val<>, dynamic_copyable_v_table>);

static_assert(!proxy_borrowable_from<val<>, cref, observeable_v_table>);
static_assert(!proxy_borrowable_from<val<>, mutref, observeable_v_table>);
static_assert(!proxy_borrowable_from<val<>, unique, observeable_v_table>);
static_assert(!proxy_borrowable_from<val<>, shared, observeable_v_table>);
static_assert(!proxy_borrowable_from<val<>, weak, observeable_v_table>);
static_assert(!proxy_borrowable_from<val<>, val<>, dynamic_copyable_v_table>);

static_assert(!cloneable_to<mutref>);
static_assert(!cloneable_to<cref>);
static_assert(cloneable_to<shared>);
static_assert(!cloneable_to<weak>);
static_assert(cloneable_to<unique>);
static_assert(cloneable_to<val<>>);

static_assert(!moveable_from<mutref, cref>);
static_assert(moveable_from<mutref, mutref>);
static_assert(!moveable_from<mutref, unique>);
static_assert(!moveable_from<mutref, shared>);
static_assert(!moveable_from<mutref, weak>);
static_assert(!moveable_from<mutref, val<>>);

static_assert(moveable_from<cref, cref>);
static_assert(moveable_from<cref, mutref>);
static_assert(!moveable_from<cref, unique>);
static_assert(!moveable_from<cref, shared>);
static_assert(!moveable_from<cref, weak>);
static_assert(!moveable_from<cref, val<>>);

static_assert(!moveable_from<shared, cref>);
static_assert(!moveable_from<shared, mutref>);
static_assert(moveable_from<shared, unique>);
static_assert(moveable_from<shared, shared>);
static_assert(!moveable_from<shared, weak>);
static_assert(!moveable_from<shared, val<>>);

static_assert(!moveable_from<weak, cref>);
static_assert(!moveable_from<weak, mutref>);
static_assert(!moveable_from<weak, unique>);
static_assert(moveable_from<weak, shared>);
static_assert(moveable_from<weak, weak>);
static_assert(!moveable_from<weak, val<>>);

static_assert(!moveable_from<unique, cref>);
static_assert(!moveable_from<unique, mutref>);
static_assert(moveable_from<unique, unique>);
static_assert(!moveable_from<unique, shared>);
static_assert(!moveable_from<unique, weak>);
static_assert(!moveable_from<unique, val<>>);

static_assert(!moveable_from<val<>, cref>);
static_assert(!moveable_from<val<>, mutref>);
static_assert(!moveable_from<val<>, unique>);
static_assert(!moveable_from<val<>, shared>);
static_assert(!moveable_from<val<>, weak>);
static_assert(moveable_from<val<>, val<>>);

static_assert(is_model_size_v_table<dynamic_copyable_v_table>);
static_assert(is_copy_constructor_v_table<dynamic_copyable_v_table>);
static_assert(is_move_constructor_v_table<dynamic_copyable_v_table>);
static_assert(is_destructor_v_table<dynamic_copyable_v_table>);
static_assert(is_delete_v_table<dynamic_deletable_v_table>);

static_assert(is_proxy_compatible_with_trait<cref, observeable>);
static_assert(!is_proxy_compatible_with_trait<val<>, observeable>);
static_assert(is_proxy_compatible_with_trait<val<>, dynamic_copyable>);

}  // namespace anyxx