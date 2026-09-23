#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>

namespace anyxx26 {

consteval bool is_const_function(std::meta::info f) {
    if(is_static_member(f)) {
        return is_const(remove_reference(type_of(parameters_of(f)[0])));
    } else {
        return is_const(type_of(f));
    }
}

}  // namespace anyxx26::meta