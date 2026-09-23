#pragma once

#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_interface/spec.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>
#include <ranges>
#include <string>   
#include <algorithm>

namespace anyxx26 {

struct overload_set_spec {
    std::string name;
    std::vector<interface_spec> specs;
};
using overload_sets_spec = std::vector<overload_set_spec>;

template <std::meta::info TraitDeclaration>
consteval overload_sets_spec make_overload_sets_specs() {
    overload_sets_spec specs;
    for(auto s : get_v_table_specs(TraitDeclaration)) {
        if(is_function_or_operator(s)) {
            std::string name{ meta::function_name_of(s.member) };
            auto found = std::ranges::find_if(specs, [&](auto const spec){ return spec.name == name; });
            if(found == specs.end()) {
                specs.push_back(overload_set_spec{ name, {s} });
            } else {
                found->specs.push_back(s);
            }
        }
    }
    return specs;
}

}  // namespace anyxx26
