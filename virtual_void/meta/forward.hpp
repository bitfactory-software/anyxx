#pragma once

#include <typeindex>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {
class i_table;

template <typename CLASS>
constexpr i_table& get_i_table_of();

constexpr const std::type_info& get_type_info(i_table const&);
}  // namespace virtual_void::meta
