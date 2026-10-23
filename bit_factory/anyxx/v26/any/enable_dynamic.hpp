#pragma once

#include <bit_factory/anyxx/v26/any/v_table.hpp>

namespace anyxx {

template <typename T, template <typename, typename, typename...> typename Trait, typename... Args>
struct enable_dynamic {
    constexpr enable_dynamic() {
        anyxx::bind_v_table_to_meta_data<v_table<Trait, Args...>, T>();
    }
};

}  // namespace anyxx
