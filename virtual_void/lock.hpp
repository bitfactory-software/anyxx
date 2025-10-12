#pragma once

#include <virtual_void/shared_const.hpp>

namespace virtual_void {

template <is_any FROM_INTERFACE>
  requires std::same_as<typename FROM_INTERFACE::erased_data_t, weak>
auto lock(FROM_INTERFACE const& from_interface) {
  using to_interface_t = FROM_INTERFACE::template type_for<shared_const>;
  static_assert(is_any<to_interface_t>);
  using return_t = std::optional<to_interface_t>;
  if (auto shared_const = get_erased_data(from_interface).lock())
    return return_t{{shared_const, get_v_table(from_interface)}};
  return return_t{};
}

};  // namespace virtual_void