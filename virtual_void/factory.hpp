#pragma once

#include <cassert>
#include <functional>
#include <map>
#include <ranges>
#include <vector>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

class unkonwn_factory_key_error : error {
  using error::error;
};

template <template <typename> typename ANY, typename KEY,
          typename... ARGS>
class factory {
  using constructor_t = std::function<ANY<unique>(ARGS...)>;
  std::map<KEY, constructor_t> function_map_;

 public:
  void register_(auto const& key, auto const& construct) {
    function_map_[key] = construct;
  }
  ANY<unique> construct(auto key, ARGS&&... args) {
    if (auto found = function_map_.find(key); found != function_map_.end())
      return construct(std::forward<ARGS>(args)...);
    throw unkonwn_factory_key_error{std::to_string(key)};
  };
};

}  // namespace virtual_void