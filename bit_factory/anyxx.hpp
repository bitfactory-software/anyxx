#pragma once

/*! \file bit_factory/anyxx.hpp
    \brief C++ header only library for external polymorphism.

    for Microsoft C++, you must enable the C-Preprocessor with this flag:
    /Zc:preprocessor (see CMakeLists.txt for example)
*/

#include <bit_factory/anyxx/config.hpp>

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
#ifdef ANYXX_26
#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/trait_as.hpp>
#else
#include <bit_factory/anyxx/proxies/static/vany.hpp>
#include <bit_factory/v23/any.hpp>
#include <bit_factory/v23/preprocessor_meta_class.hpp>
#include <bit_factory/v23/v_table_data.hpp>
#include <bit_factory/v23/v_table_holder/v_table_holder_dyn.hpp>
#include <bit_factory/v23/v_table_holder/v_table_holder_static.hpp>
#endif  // !ANYXX_26


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

