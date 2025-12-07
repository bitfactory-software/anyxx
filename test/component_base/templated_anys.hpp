#pragma once

#include <bit_factory/anyxx.hpp>
#include <map>  // NOLINT
#include <string>

#include "component_base.hpp"

#define __ANY_TEMPLATE_MODEL_FWD(export_, class_, interface_namespace_,        \
                                 interface_, t, all)                           \
  namespace interface_namespace_ {                                             \
  template <>                                                                  \
      export_ interface_##_v_table _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS( \
          t) *                                                                 \
      _detail_ANYXX_MAKE_V_TABLE_FUNCTION_NAME(                                \
          interface_)<_detail_ANYXX_TEMPLATE_ARGS(all)>();                     \
  }

#define ANY_TEMPLATE_MODEL_FWD(export_, class_, interface_namespace_,         \
                               interface_, t)                                 \
  __ANY_TEMPLATE_MODEL_FWD(export_, class_, interface_namespace_, interface_, \
                           t, _add_head(class_, t))

namespace test::component_base {

ANY_TEMPLATE(((KEY), (VALUE)), any_map,
             (ANY_METHOD(VALUE const&, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE_(((KEY), (VALUE)), any_mutable_map, any_map, (KEY, VALUE),
              (ANY_METHOD_OVERLOAD(VALUE&, at, (KEY), ),
               ANY_OP(VALUE&, [], (KEY), )))

ANY_TEMPLATE(((KEY), (VALUE)), any_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)))

ANY_TEMPLATE(((KEY), (VALUE)), any_mutable_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), ),
              ANY_METHOD(std::size_t, size, (), const)))

}  // namespace test::component_base

ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<int, double>)
ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<std::string, int>)
ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<std::string, double>)

ANY_TEMPLATE_MODEL_FWD(COMPONENT_BASE_EXPORT, (std::map<std::string, int>),
                       test::component_base, any_mutable_map,
                       ((std::string), (int)))
ANY_TEMPLATE_MODEL_FWD(COMPONENT_BASE_EXPORT, (std::map<int, double>),
                       test::component_base, any_mutable_map,
                       ((int), (double)))
ANY_TEMPLATE_MODEL_FWD(COMPONENT_BASE_EXPORT, (std::map<int, double>),
                       test::component_base, any_map,
                       ((int), (double)))
