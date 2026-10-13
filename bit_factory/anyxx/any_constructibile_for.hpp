#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/any_forward.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <concepts>
#include <type_traits>

namespace anyxx {

template <typename ConstructedWith, typename Proxy, typename Base>
concept constructibile_for =
(proxy_trait<Proxy>::template is_constructibile_from<
    ConstructedWith>::value) ||
    (erased_constructibile_for<ConstructedWith, Proxy, Base> &&
        !is_proxy_holder<ConstructedWith> &&
        !is_typed_any<std::remove_cvref_t<ConstructedWith>>);

}  // namespace anyxx
