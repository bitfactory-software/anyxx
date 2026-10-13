#pragma once

#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>

namespace anyxx {

// --------------------------------------------------------------------------------
// any customization traits

template <is_proxy Proxy = val<>>
struct default_proxy {
  using type = Proxy;
};

}  // namespace anyxx