#pragma once

#include <bit_factory/anyxx/utilities.hpp>
#include <bit_factory/anyxx/v26/any/keywords.hpp>
#include <bit_factory/anyxx/v26/trait_translation/find_candidate_in_target.hpp>

namespace anyxx {

consteval std::vector<std::meta::info> make_trait_as_params_for_variant(std::meta::info v, auto args) {
    std::vector<std::meta::info> types;
    types.push_back(v);
    types.append_range(args);
    return types;
}
consteval std::meta::info make_trait_as_for_variant(std::meta::info v, auto args) {
    return substitute(^^trait_as, make_trait_as_params_for_variant(v, args));
}
consteval std::vector<std::meta::info> extract_trait_as_params(std::meta::info jacket) {
    auto jacket_template = template_of(jacket);
    auto jacket_template_arguments = template_arguments_of(jacket);
    if (jacket_template == ^^trait_as || jacket_template == ^^any_) {
        return std::ranges::to<std::vector>(jacket_template_arguments | std::views::drop(1));
    } else
    {
        return jacket_template_arguments;
    }
}
template <std::meta::info Spec, std::meta::info jacket, typename R, typename V, typename... Args>
constexpr R invoke_variant(V&& v, Args... args){
    return std::visit(overloads{
        [&] <typename T>([[maybe_unused]] T const& x) {
            [[maybe_unused]] constexpr auto trait_args = std::define_static_array(extract_trait_as_params(jacket));
            using delegate_trait_as = typename[:make_trait_as_for_variant(^^T, trait_args):];
            [[maybe_unused]] constexpr auto target = find_candidate_in<delegate_trait_as, Spec>();
            return delegate_trait_as{ x }.[:target:](args...);
        },[&]<is_any Any>(Any const& x) {
            [[maybe_unused]] constexpr auto target = find_candidate_in<Any, Spec>();
            return x.[:target:](args...);
        }}, std::forward<V>(v));
}

}  // namespace anyxx
