#pragma once

#include "../../../m_table/m_table.h"
#include "../holder.h"

namespace virtual_void::erased::data {

struct has_m_table {
  m_table_t* m_table_ = nullptr;

  const has_m_table* meta() const { return this; }
  
  template <typename T>
  has_m_table(std::in_place_type_t<T>)
      : m_table_(m_table_of<std::decay_t<T>>()) {}

  has_m_table() noexcept = default;
  has_m_table(const has_m_table&) = default;
  has_m_table(has_m_table&) = default;
  has_m_table(has_m_table&& rhs) noexcept { swap(*this, rhs); }
  has_m_table& operator=(has_m_table&& rhs) noexcept {
    has_m_table destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(has_m_table& lhs, has_m_table& rhs) noexcept {
    using namespace std;
    swap(lhs.m_table_, rhs.m_table_);
  }

  type_info_ptr type_info() const { return &get_m_table()->type(); }
  m_table_t* get_m_table() const { return m_table_; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_m_table() = default;
};

using with_has_m_table = base<has_m_table>;

}  // namespace virtual_void::erased::data