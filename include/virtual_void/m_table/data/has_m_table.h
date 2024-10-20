#pragma once

#include "../m_table.h"

namespace virtual_void::m_table::data {

struct has_m_table {
  m_table_t* m_table_;
  template <typename T>
  has_m_table(std::in_place_type_t<T>)
      : m_table_(m_table_of<std::decay_t<T>>()) {}
  type_info_ptr type_info() const { return &get_m_table()->type(); }
  m_table_t* get_m_table() const { return m_table_; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_m_table() = default;
};

}  // namespace virtual_void::erased::data