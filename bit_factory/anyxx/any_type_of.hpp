#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {

template <is_any Any>
  requires is_type_info_v_table<typename Any::v_table_t>
inline std::type_info const& type_of(Any const& any_) {
  return *get_v_table(any_)->type_info_;
}

}  // namespace anyxx