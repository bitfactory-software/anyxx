#pragma once

#include <utility>

namespace anyxx {

template <typename VTable, typename Concrete>
VTable* v_table_instance() {
  static VTable v_table{std::in_place_type<Concrete>};
  return &v_table;
}


}  // namespace anyxx