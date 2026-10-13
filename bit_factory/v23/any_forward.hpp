#pragma once

#include <bit_factory/anyxx/proxies/proxy_concept.hpp>

namespace anyxx {

template <typename Trait, is_proxy Proxy = typename Trait::default_proxy_t>
class any;

template <typename TraitDerived, is_proxy ProxyDerived, typename TraitBase,
    is_proxy ProxyBase>
constexpr bool is_any_derived_from_v<any<TraitDerived, ProxyDerived>,
    any<TraitBase, ProxyBase>> =
    std::derived_from<typename any<TraitDerived, ProxyDerived>::v_table_t,
    typename any<TraitBase, ProxyBase>::v_table_t>;

}  // namespace anyxx