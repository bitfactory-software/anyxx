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
#include <bit_factory/anyxx/borrow_as.hpp>
#include <bit_factory/anyxx/clone_to.hpp>
#include <bit_factory/anyxx/compute_val_proxy_size.hpp>
#include <bit_factory/anyxx/crosscast_runtime.hpp>
#include <bit_factory/anyxx/dll_mode.hpp>
#include <bit_factory/anyxx/downcast.hpp>
#include <bit_factory/anyxx/lock.hpp>
#include <bit_factory/anyxx/move_to.hpp>
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

