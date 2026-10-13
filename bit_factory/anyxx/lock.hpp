#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/weak.hpp>
#include <optional>

namespace anyxx {

/// \brief Lock a shared \ref any.
/// \ingroup casts
template <is_any FromAny>
  requires std::same_as<typename FromAny::proxy_t, weak>
auto lock(FromAny const& from_interface) {
  using to_any_t = any<typename FromAny::trait_t, shared>;
  static_assert(is_any<to_any_t>);
  using return_t = std::optional<to_any_t>;
  if (auto locked = get_proxy(from_interface).lock())
    return return_t{to_any_t{std::move(locked), get_v_table(from_interface)}};
  return return_t{};
}

}  // namespace anyxx