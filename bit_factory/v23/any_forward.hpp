#pragma once

#include <bit_factory/anyxx/proxies/proxy_concept.hpp>

namespace anyxx {

template <typename...>
class any;

template <typename Derived, typename Base>
struct is_any_derived_from_v {
	static constexpr bool value = std::derived_from<typename Derived::v_table_t, typename Base::v_table_t>;
};

}  // namespace anyxx