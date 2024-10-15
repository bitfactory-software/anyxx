#pragma once

#include <typeindex>

#include "../forward.h"
#include "../open_method/domain.h"
#include "../open_method/table.h"

namespace virtual_void::typeid_ {

class open_method_base;
using domain = virtual_void::open_method::domain<open_method_base>;

class open_method_base : public virtual_void::open_method::table {
 protected:
  using dispatch_target_index_t =
      perfect_typeid_hash::index_table<dispatch_target_t>;
  std::unique_ptr<dispatch_target_index_t> dispatch_target_index_;

 public:
  open_method_base() = default;
  explicit open_method_base(domain& domain) {
    domain.open_methods.push_back(this);
  }
  void seal_for_runtime() {
    dispatch_target_index_ =
        std::make_unique<dispatch_target_index_t>(make_lookup_table());
  }
  template <typename ERASED_FUNCTION_TYPE>
  auto lookup(const std::type_info& type_info) const {
    if (!dispatch_target_index_) throw error("Not yet sealed for runtime.");
    return reinterpret_cast<ERASED_FUNCTION_TYPE>(
        dispatch_target_index_->at(&type_info));
  }
};

inline void seal_for_runtime(domain& domain) {
  for (const auto& method : domain.open_methods) method->seal_for_runtime();
}

}  // namespace virtual_void::typeid_