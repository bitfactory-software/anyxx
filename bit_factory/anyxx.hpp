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
}  // namespace anyxx

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