#pragma once

#include <map>
#include <optional>
#include <typeindex>
#include <vector>

#include <virtual_void/data/erased_value.hpp>

namespace virtual_void::open_object {

template <typename OBJECT_TYPE>
std::size_t type_member_count_of = 0;

template <typename OBJECT_TYPE>
struct members {
  members() : table_(type_member_count_of<OBJECT_TYPE>) {}
  std::vector<data::erased_value> table_;
  template <typename OBJECT_MEMBER, typename ARG>
  void set(OBJECT_MEMBER member, ARG&& arg) {
    using value_t = typename OBJECT_MEMBER::value_t;
    table_[member.index] =
        data::make_erased_value<value_t>(std::forward<ARG>(arg));
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t const* get(OBJECT_MEMBER member) const {
    const auto& value = table_[member.index];
    if (!value) return {};
    return static_cast<typename OBJECT_MEMBER::value_t const*>(value.get());
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t* get(OBJECT_MEMBER member) {
    auto& value = table_[member.index];
    if (!value) return {};
    return static_cast<typename OBJECT_MEMBER::value_t*>(value.get());
  }
  template <typename OBJECT_MEMBER>
  typename OBJECT_MEMBER::value_t& operator[](OBJECT_MEMBER member) {
    if (auto value = get(member)) {
      return *value;
    }
    using value_t = typename OBJECT_MEMBER::value_t;
    set(member, value_t());
    return *get(member);
  }
};

template <typename OBJECT_TYPE, typename VALUE_TYPE>
struct member {
  using object_t = OBJECT_TYPE;
  using value_t = VALUE_TYPE;
  std::size_t index = type_member_count_of<OBJECT_TYPE>++;
};

}  // namespace virtual_void::open_object