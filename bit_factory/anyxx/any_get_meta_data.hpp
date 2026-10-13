#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {


template <is_any Any>
  requires is_meta_data_v_table<typename Any::v_table_t>
inline const auto& get_meta_data(Any const& any) {
  return *get_v_table(any)->meta_data_;
}

}  // namespace anyxx