#pragma once

#include <map>
#include <typeindex>
#include <vector>

#include "../erased/lifetime/value.h"

namespace virtual_void::open_object {

template <typename OBJECT_TYPE>
int& type_member_index() {
  static int c;
  return c;
}

template <typename OBJECT_TYPE>
struct members {
  std::vector<erased::value> table_;
  template <typename OBJECT_MEMBER>
  auto operator[](OBJECT_MEMBER) {
    return reconcrete_cast<OBJECT_MEMBER::MEMBER_TYPE>(
        table_[OBJECT_MEMBER::index::value]);
  }
  template <typename OBJECT_MEMBER>
  auto operator[](OBJECT_MEMBER) const {
    return reconcrete_cast<OBJECT_MEMBER::MEMBER_TYPE>(
        table_[OBJECT_MEMBER::index::value]);
  }
};

template <typename OBJECT_TYPE>
struct member_table_index {
  const int value;
  member_table_index() : value(type_member_count_of<OBJECT_TYPE>()++) {}
};

template <typename OBJECT_TYPE, typename MEMBER_TYPE, typename VALUE_TYPE>
struct member {
  using object_t = OBJECT_TYPE;
  using member_t = MEMBER_TYPE;
  using value_t = VALUE_TYPE;
  const member_table_index<OBJECT_TYPE> index;
};

}  // namespace virtual_void::open_object