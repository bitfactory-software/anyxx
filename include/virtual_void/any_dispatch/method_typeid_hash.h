#pragma once

#include <any>
#include <functional>
#include <map>
#include <typeindex>

#include "virtual_void/perfect_typeid_hash/index_table.h"

namespace any_dispatch {
template <typename R, typename ANY, typename... ARGS>
class method_typeid_hash {
  using erased_function_t = std::function<R(ANY, ARGS...)>;

 private:
  std::map<perfect_typeid_hash::type_id, erased_function_t> dispatchMap_;
  perfect_typeid_hash::index_table<erased_function_t> indexTable_;
  erased_function_t fallback_ = [](ANY dispatch, ARGS... args) -> R {
    throw std::runtime_error(std::string(dispatch.type().name()) +
                             ": No registered method.");
  };

 public:
  auto define_erased(const std::type_info& register_type_info,
                     const erased_function_t& f) {
    auto entry = dispatchMap_.find(&register_type_info);
    if (entry != dispatchMap_.end())
      throw std::runtime_error("Method for type already registered.");
    dispatchMap_[&register_type_info] = f;
    return nullptr;
  }
  auto define_fallback(const erased_function_t& fallback) {
    fallback_ = fallback;
  }
  template <typename DISPATCH>
  auto define(const auto& f) {
    return define_erased(typeid(DISPATCH),
                         [f](ANY dispatch, ARGS... args) -> R {
                           auto unerased = std::any_cast<DISPATCH>(&dispatch);
                           return f(unerased, std::forward<ARGS>(args)...);
                         });
  }
  R operator()(ANY dispatch, ARGS... args) const {
    auto target = indexTable_[&dispatch.type()];
    return target(dispatch, args...);
  }
  erased_function_t is_defined(const std::type_info& type_info) {
    if (auto entry = dispatchMap_.find(&type_info); entry != dispatchMap_.end())
      return entry->second;
    return nullptr_t{};
  }
  template <typename C>
  erased_function_t is_defined() {
    return is_defined(typeid(C));
  }
  void seal() {
    indexTable_.build(dispatchMap_, fallback_);
    dispatchMap_.clear();
  }
};
}  // namespace any_dispatch
