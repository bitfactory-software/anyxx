#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {

template <is_any Any>
  requires is_dynamic_castable_v_table<typename Any::v_table_t>
bool is_derived_from(const std::type_info& from, Any const& any_) {
  return get_v_table(any_)->is_derived_from_(from);
}

}  // namespace anyxx