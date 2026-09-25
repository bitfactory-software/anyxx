#pragma once

#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_translation/get_implementation_member.hpp>
#include <bit_factory/v26/trait_translation/find_candidate_in_target.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <meta>
#include <vector>

namespace anyxx26 {

struct interface_spec_with_target : interface_spec {
    std::meta::info target;
};
template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::vector<interface_spec_with_target> make_interface_specs_with_target() {
    std::vector<interface_spec_with_target> interface_specs_with_target;
    template for(constexpr auto spec : define_static_array(get_interface_specs(^^ trait_declaration_t<Trait, Args...>))){
        auto args = std::define_static_array(template_arguments_of(spec.declaration_trait) | std::views::drop(2));
        auto implementation_member = find_function_impl(spec.member, ^^ Trait, ^^ V, args);
        if(implementation_member != spec.member) {
            interface_specs_with_target.push_back(interface_spec_with_target{ spec.member, spec.index, implementation_member });
        } else {
            auto target = find_candidate_in<V, spec.member>();
            if(target != std::meta::info{}) {
                interface_specs_with_target.push_back(interface_spec_with_target{ spec.member, spec.index, target });
            }
        }
    }
    return interface_specs_with_target;
}

struct overload_set_spec_with_target {
    std::string name;
    std::vector<interface_spec_with_target> specs;
};
using overload_sets_spec_with_target = std::vector<overload_set_spec_with_target>;

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval overload_sets_spec_with_target make_overload_sets_specs_with_target() {
    overload_sets_spec_with_target specs;
    for(auto s : make_interface_specs_with_target<V, Trait, Args...>()) {
        if(is_function_or_operator(s)) {
            std::string name{ meta::function_name_of(s.member) };
            auto found = std::ranges::find_if(specs, [&](auto const spec){ return spec.name == name; });
            if(found == specs.end()) {
                specs.push_back(overload_set_spec_with_target{ name, {s} });
            } else {
                found->specs.push_back(s);
            }
        }
    }
    return specs;
}

}  // namespace anyxx26
