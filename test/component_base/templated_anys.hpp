#pragma once

#include <bit_factory/anyxx.hpp>
#include <map>  // NOLINT
#include <string>

#include "component_base.hpp"

namespace test::component_base {

ANY_TEMPLATE(((KEY), (VALUE)), map,
             (ANY_METHOD_EXACT(VALUE const&, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)), anyxx::const_observer)

ANY_TEMPLATE_(((KEY), (VALUE)), mutable_map, map, ((KEY), (VALUE)),
              (ANY_METHOD_OVERLOAD_EXACT(VALUE&, at, (KEY), ),
               ANY_OP(VALUE&, [], (KEY), )), anyxx::mutable_observer)

ANY_TEMPLATE(((KEY), (VALUE)), recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), const),
              ANY_METHOD(std::size_t, size, (), const)), anyxx::const_observer)

ANY_TEMPLATE(((KEY), (VALUE)), mutable_recursive_map,
             (ANY_METHOD(VALUE, at, (KEY), ),
              ANY_METHOD(std::size_t, size, (), const)), anyxx::const_observer)

}  // namespace test::component_base

ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<int, double>)
ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<std::string, int>)
ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<std::string, double>)

