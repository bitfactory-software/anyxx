#pragma once

#include <assert.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../m_table.h"

namespace virtual_void::m_table {

struct abstract_data {
  const m_table_t* m_table_ = nullptr;
  void* data_ = nullptr;
  abstract_data(const m_table_t* m_table, void* data)
      : m_table_(m_table), data_(data) {}
};
template <typename M>
struct concrete_data : abstract_data {
  M m_;
  concrete_data(M&& m)
      : abstract_data(m_table_of<M>(), std::addressof(m_)), m_(std::move(m)) {}
  template <typename... ARGS>
  concrete_data(std::in_place_t, ARGS&&... args)
      : abstract_data(m_table_of<M>(), std::addressof(m_)),
        m_(std::forward<ARGS>(args)...) {}
};

}  // namespace virtual_void::m_table