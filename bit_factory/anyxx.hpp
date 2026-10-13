#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/ANY_UNIQUE_NAME.hpp>
#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/any_get_meta_data.hpp>
#include <bit_factory/anyxx/any_get_type_info.hpp>
#include <bit_factory/anyxx/any_unerase_cast.hpp>
#include <bit_factory/anyxx/compute_val_proxy_size.hpp>
#include <bit_factory/anyxx/crosscast_runtime.hpp>
#include <bit_factory/anyxx/dll_mode.hpp>
#include <bit_factory/anyxx/downcast.hpp>
#include <bit_factory/anyxx/observeable.hpp>
#include <bit_factory/anyxx/small_object_size.hpp>
#include <bit_factory/anyxx/utilities.hpp>
#include <bit_factory/anyxx/v_table_instance.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>
#include <bit_factory/anyxx/voidness.hpp>
#include <bit_factory/anyxx/errors/type_mismatch_error.hpp>
#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/proxies/unerase_cast.hpp>
#include <bit_factory/anyxx/proxies/dyn/cow.hpp>
#include <bit_factory/anyxx/proxies/dyn/observer.hpp>
#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/weak.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>
#include <bit_factory/anyxx/proxies/dyn/borrowable_from.hpp>
#include <bit_factory/anyxx/proxies/dyn/cloneable_to.hpp>
#include <bit_factory/anyxx/proxies/dyn/moveable_from.hpp>
#include <bit_factory/anyxx/proxies/static/trait_class.hpp>
#include <bit_factory/anyxx/proxies/static/using_cref.hpp>
#include <bit_factory/anyxx/proxies/static/using_.hpp>
#include <bit_factory/anyxx/proxies/static/vany.hpp>
#include <bit_factory/infra/config.hpp>
#include <bit_factory/v23/any.hpp>
#include <bit_factory/v23/preprocessor_meta_class.hpp>
#include <bit_factory/v23/v_table_holder/v_table_holder_dyn.hpp>
#include <bit_factory/v23/v_table_holder/v_table_holder_static.hpp>

namespace anyxx {

struct dynamic_castable;
struct dynamic_deletable;
struct dynamic_moveable;
struct dynamic_copyable;

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
      } else if constexpr (is_any<sig_t> && Any::is_dyn) {
        return unchecked_downcast_to<Any>(std::forward<Sig>(sig));
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
// any borrow, clone, lock, move

template <is_any ToAny, is_proxy FromProxy, typename FromVTable>
  requires borrowable_from<typename ToAny::proxy_t, FromProxy, FromVTable>
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
  requires borrowable_from<typename ToAny::proxy_t,
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

}  // namespace anyxx

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
static_assert(!borrowable_from<mutref, cref, observeable_v_table>);
static_assert(borrowable_from<mutref, mutref, observeable_v_table>);
static_assert(borrowable_from<mutref, unique, observeable_v_table>);
static_assert(!borrowable_from<mutref, shared, observeable_v_table>);
static_assert(!borrowable_from<mutref, weak, observeable_v_table>);
static_assert(borrowable_from<mutref, val<>, dynamic_copyable_v_table>);

static_assert(borrowable_from<cref, cref, observeable_v_table>);
static_assert(borrowable_from<cref, mutref, observeable_v_table>);
static_assert(borrowable_from<cref, unique, observeable_v_table>);
static_assert(borrowable_from<cref, shared, observeable_v_table>);
static_assert(!borrowable_from<cref, weak, observeable_v_table>);
static_assert(borrowable_from<cref, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<shared, cref, observeable_v_table>);
static_assert(!borrowable_from<shared, mutref, observeable_v_table>);
static_assert(!borrowable_from<shared, unique, observeable_v_table>);
static_assert(borrowable_from<shared, shared, observeable_v_table>);
static_assert(!borrowable_from<shared, weak, observeable_v_table>);
static_assert(!borrowable_from<shared, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<weak, cref, observeable_v_table>);
static_assert(!borrowable_from<weak, mutref, observeable_v_table>);
static_assert(!borrowable_from<weak, unique, observeable_v_table>);
static_assert(borrowable_from<weak, shared, observeable_v_table>);
static_assert(borrowable_from<weak, weak, observeable_v_table>);
static_assert(!borrowable_from<weak, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<unique, cref, observeable_v_table>);
static_assert(!borrowable_from<unique, mutref, observeable_v_table>);
static_assert(!borrowable_from<unique, unique, observeable_v_table>);
static_assert(!borrowable_from<unique, shared, observeable_v_table>);
static_assert(!borrowable_from<unique, weak, observeable_v_table>);
static_assert(!borrowable_from<unique, val<>, dynamic_copyable_v_table>);

static_assert(!borrowable_from<val<>, cref, observeable_v_table>);
static_assert(!borrowable_from<val<>, mutref, observeable_v_table>);
static_assert(!borrowable_from<val<>, unique, observeable_v_table>);
static_assert(!borrowable_from<val<>, shared, observeable_v_table>);
static_assert(!borrowable_from<val<>, weak, observeable_v_table>);
static_assert(!borrowable_from<val<>, val<>, dynamic_copyable_v_table>);

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

}  // namespace anyxx