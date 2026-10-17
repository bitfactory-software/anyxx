#pragma once

#include <bit_factory/anyxx.hpp>
#include <map>  // NOLINT
#include <string>

#include "component_base.hpp"

namespace test::component_base {

TRAIT_TEMPLATE(((KEY), (VALUE)), map, anyxx::dynamic_copyable, (),
               (FN_EXACT(VALUE const&, at, (KEY), const),
                FN(std::size_t, size, (), const)))

TRAIT_TEMPLATE(((KEY), (VALUE)), mutable_map, map, ((KEY), (VALUE)),
               (FN_OVERLOAD_EXACT(VALUE&, at, (KEY), ),
                OP_(VALUE&, [], (KEY), )))

TRAIT_TEMPLATE(((KEY), (VALUE)), recursive_map, anyxx::dynamic_copyable, (),
               (FN(VALUE, at, (KEY), const), FN(std::size_t, size, (), const)))

TRAIT_TEMPLATE(((KEY), (VALUE)), mutable_recursive_map, anyxx::dynamic_copyable,
               (), (FN(VALUE, at, (KEY), ), FN(std::size_t, size, (), const)))

}  // namespace test::component_base

ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<int, double>)
ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<std::string, int>)
ANY_META_CLASS_FWD(COMPONENT_BASE_EXPORT, std::map<std::string, double>)
