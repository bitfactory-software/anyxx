#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>
#include <bit_factory/anyxx/v23/any.hpp>
#include <bit_factory/anyxx/v23/preprocessor_meta_class.hpp>
#include <bit_factory/anyxx/v23/v_table_holder/v_table_holder_dyn.hpp>
#include <bit_factory/anyxx/v23/v_table_holder/v_table_holder_static.hpp>

namespace anyxx {

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

TRAIT_EX(const_referenceable, observeable, , , , ,
         (using default_proxy_t = cref;));
TRAIT_EX(mutable_referenceable, observeable, , , , ,
         (using default_proxy_t = mutref;));
TRAIT_EX(deleteable, observeable, , , , , (using default_proxy_t = unique;));

TRAIT_EX(moveable, observeable, , , ,
         (ANY_MODEL_SIZE, ANY_MOVE_CONSTRUCTOR, ANY_DESTRUCTOR),
         (using default_proxy_t = val<>;));

TRAIT_EX(copyable, moveable, , , , (ANY_COPY_CONSTRUCTOR), ());

TRAIT_EX(typed_observeable, observeable, , , , (ANY_CLASS_TYPE_INFO), ());

TRAIT_EX(typed_moveable, typed_observeable, , , ,
         (ANY_MODEL_SIZE, ANY_MOVE_CONSTRUCTOR, ANY_DESTRUCTOR),
         (using default_proxy_t = val<>;));

TRAIT_EX(typed_copyable, typed_moveable, , , , (ANY_COPY_CONSTRUCTOR), ());

TRAIT_EX(dynamic_castable, typed_observeable, , , ,
         (ANY_CAN_TYPE_SAVE_DOWNCAST, ANY_CAN_TYPE_SAVE_CROSSCAST), ());

TRAIT_EX(dynamic_deletable, dynamic_castable, , , , (ANY_HAS_DELETE),
         (using default_proxy_t = shared;));

TRAIT_EX(dynamic_smart_ptr, dynamic_deletable, , , ,
         (ANY_MODEL_SIZE, ANY_COPY_CONSTRUCTOR), ());

TRAIT_EX(dynamic_moveable, dynamic_castable, , , ,
         (ANY_MODEL_SIZE, ANY_MOVE_CONSTRUCTOR, ANY_DESTRUCTOR),
         (using default_proxy_t = val<>;));

TRAIT_EX(dynamic_copyable, dynamic_moveable, , , , (ANY_COPY_CONSTRUCTOR), ());
}  // namespace anyxx
