#pragma once

#include <map>
#include <optional>
#include <typeindex>
#include <vector>

#include "erased_value.h"

namespace virtual_void::open_object {

template <typename OBJECT_TYPE>
std::size_t& type_member_count_of() {
  static std::size_t c;
  return c;
}

template <typename OBJECT_TYPE>
struct members {
  members() : table_(type_member_count_of<OBJECT_TYPE>()) {}
  std::vector<data::erased_value<void>> table_;
  template <typename OBJECT_MEMBER, typename ARG>
  void set(OBJECT_MEMBER, ARG&& arg) {
    using value_t = typename OBJECT_MEMBER::value_t;
    table_[OBJECT_MEMBER::get_index()] =
        data::make_erased_value<void, value_t>(std::forward<ARG>(arg));
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t const* get(OBJECT_MEMBER) const {
    const auto& value = table_[OBJECT_MEMBER::get_index()];
    if (!value) return {};
    return static_cast<typename OBJECT_MEMBER::value_t const*>(value.get());
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t* get(OBJECT_MEMBER) {
    auto& value = table_[OBJECT_MEMBER::get_index()];
    if (!value) return {};
    return static_cast<typename OBJECT_MEMBER::value_t*>(value.get());
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t& operator[](OBJECT_MEMBER) {
    if (auto value = get(OBJECT_MEMBER())) {
      return *value;
    }
    using value_t = typename OBJECT_MEMBER::value_t;
    set(OBJECT_MEMBER{}, value_t());
    return *get(OBJECT_MEMBER{});
  }
};

template <typename OBJECT_TYPE>
struct member_table_index {
  const std::size_t value;
  member_table_index() : value(type_member_count_of<OBJECT_TYPE>()) {
    ++type_member_count_of<OBJECT_TYPE>();
  }
};

template <typename OBJECT_TYPE, typename MEMBER_TYPE, typename VALUE_TYPE>
struct member {
  using object_t = OBJECT_TYPE;
  using member_t = MEMBER_TYPE;
  using value_t = VALUE_TYPE;
  static const member_table_index<OBJECT_TYPE> index;
  static std::size_t get_index() { return member_t::index.value; }
};

}  // namespace virtual_void::open_object