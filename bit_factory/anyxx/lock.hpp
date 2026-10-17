#pragma once

#include <bit_factory/anyxx/any_access.hpp>
#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/weak.hpp>
#include <optional>

namespace anyxx {

/// \brief Lock a shared \ref any_.
/// \ingroup casts
template <is_any ToAny, is_any FromAny>
  requires std::same_as<typename FromAny::proxy_t, weak>
auto lock(FromAny const& from_interface) {
  using return_t = std::optional<ToAny>;
  if (auto locked = get_proxy(from_interface).lock())
    return return_t{ ToAny{std::move(locked), get_v_table(from_interface)}};
  return return_t{};
}

}  // namespace anyxx