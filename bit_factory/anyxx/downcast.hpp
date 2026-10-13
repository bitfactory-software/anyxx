#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {

template <is_any Any>
    requires is_dynamic_castable_v_table<typename Any::v_table_t>
bool is_derived_from(const std::type_info& from, Any const& any) {
    return get_v_table(any)->is_derived_from_(from);
}

template <is_any From, is_any Any>
    requires is_dynamic_castable_v_table<typename From::v_table_t>&&
is_dynamic_castable_v_table<typename Any::v_table_t>
bool is_derived_from(Any const& any) {
    return is_derived_from(typeid(typename From::v_table_t), any);
}

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

}  // namespace anyxx