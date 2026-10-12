#pragma once

#include <bit_factory/anyxx/utilities.hpp>

namespace anyxx {

template <typename T>
constexpr std::size_t compute_val_proxy_size(std::size_t default_size) {
    if constexpr(anyxx::is_type_complete<T>) {
        return T::value;
    } else {
        return default_size;
    };
}

}  // namespace anyxx