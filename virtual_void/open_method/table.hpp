#pragma once

#include <typeindex>
#include <unordered_map>

#include "../perfect_typeid_hash/index_table.hpp"
#include "../virtual_void.hpp"
#include "default_target.hpp"

namespace virtual_void::open_method {

class table : default_target<> {
 public:
  using dispatch_target_t = default_target<>::dispatch_target_t;
  using dispatch_target_index_t =
      perfect_typeid_hash::index_table<dispatch_target_t>;

 private:
  using method_map_t =
      std::unordered_map<std::type_info const*, dispatch_target_t>;
  method_map_t map_;

 public:
  auto define_erased(const std::type_info& register_type_info, auto f) {
    auto t = reinterpret_cast<dispatch_target_t>(f);
    if (is_defined(register_type_info))
      throw error("Method for type already registered.");
    map_[&register_type_info] = t;
    return definition{};
  }
  template <typename TARGET = dispatch_target_t>
  TARGET is_defined(const std::type_info& type_info) const {
    if (auto found = map_.find(&type_info); found != map_.end())
      return reinterpret_cast<TARGET>(found->second);
    return nullptr;
  }
  template <typename C>
  auto is_defined() const {
    return is_defined(typeid(C));
  }
  dispatch_target_index_t make_lookup_table() const {
    return dispatch_target_index_t{map_, get_default()};
  }
  struct definition {};
};

}  // namespace virtual_void::open_method