#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec_with_target.hpp>
#include <bit_factory/v26/trait_translation/get_implementation_member.hpp>
#include <bit_factory/v26/trait_translation/is_defaulted_function_spec.hpp>
#include <bit_factory/v26/trait_translation/find_candidate_in_target.hpp>
#include <bit_factory/v26/trait_translation/deduced_typenames.hpp>
#include <bit_factory/v26/trait_as/trait_calls.hpp>
#include <bit_factory/v26/trait_translation/trait_facade_decorator.hpp>
#include <meta>

namespace anyxx26 {

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info static_facade_named_overloaded_calls(overload_set_spec_with_target const& spec){
    std::vector<std::meta::info> overloads;
    for(auto overload : std::define_static_array(spec.specs)) {
        if(auto call = make_static_facade_call<V, Trait, Args...>(overload); call != std::meta::info{}) {
            overloads.push_back(call);
        }
    }
    return std::meta::data_member_spec(meta::overloaded_calls(overloads), { .name = spec.name, .no_unique_address = true });
}

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval auto static_facade_overloaded_calls() {
    std::vector<std::meta::info> calls;
    for(auto const& set : make_overload_sets_specs_with_target<V, Trait, Args...>()) {
        calls.push_back(reflect_constant(static_facade_named_overloaded_calls<V, Trait, Args...>(set)));
    }
    return calls;
}

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_trait_facade() {
    return meta::make_struct_with(static_facade_overloaded_calls<V, Trait, Args...>());
};

}  // namespace anyxx26
