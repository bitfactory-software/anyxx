#pragma once

#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

template <template <typename, typename, typename...> typename Trait, typename... Args>
concept has_deduced_typenames = requires { typename Trait<declaration, declaration, Args...>::typenames; };

template <template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info compute_deduced_typenames() {
	if constexpr(has_deduced_typenames<Trait, Args...>) {
		return ^^typename Trait<declaration, declaration, Args...>::typenames;
	} else {
		return ^^empty_t;
	}
}
template <template <typename, typename, typename...> typename Trait, typename... Args>
using deduced_typenames = [:compute_deduced_typenames<Trait, Args...>():];

}  // namespace anyxx26
