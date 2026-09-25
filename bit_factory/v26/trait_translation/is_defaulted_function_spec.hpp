#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

consteval bool is_defaulted_function_spec(std::meta::info f) {
    return !is_static_member(f) || annotations_of_with_type(f, ^^default_t).size() > 0;
}

}  // namespace anyxx26::meta