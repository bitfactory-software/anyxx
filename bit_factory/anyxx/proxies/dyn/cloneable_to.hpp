#pragma once

#include <bit_factory/anyxx/proxies/dyn/cow.hpp>
#include <bit_factory/anyxx/proxies/dyn/observer.hpp>
#include <bit_factory/anyxx/proxies/dyn/unique.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/weak.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>

namespace anyxx {

template <is_proxy To>
struct can_copy_to;

template <typename To>
concept cloneable_to = is_proxy<To> && proxy_trait<To>::is_owner;

template <is_proxy To, is_proxy From>
  requires cloneable_to<To>
To clone_to(From const& from, auto v_table) {
  return proxy_trait<To>::clone_from(get_proxy_ptr(from, v_table), v_table);
}

}  // namespace anyxx