#pragma once

#include <map>
#include <optional>
#include <typeindex>
#include <vector>

#include "../erased/lifetime/value.h"

namespace virtual_void::open_object {

template <typename OBJECT_TYPE>
std::size_t& type_member_count_of() {
  static std::size_t c;
  return c;
}

template <typename OBJECT_TYPE>
struct members {
  members() : table_(type_member_count_of<OBJECT_TYPE>()) {}
  std::vector<std::optional<erased::value>> table_;
  template <typename OBJECT_MEMBER, typename ARG>
  void set(OBJECT_MEMBER, ARG&& arg) {
    table_[OBJECT_MEMBER::get_index()] =
        erased::make_value<typename OBJECT_MEMBER::value_t>(
            std::forward<ARG>(arg));
  }
  template <typename OBJECT_MEMBER>
  std::optional<typename OBJECT_MEMBER::value_t> get(OBJECT_MEMBER) const {
    if (auto value = table_[OBJECT_MEMBER::get_index()])
      return *reconcrete_cast<std::string>(*value);
    else
      return {};
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