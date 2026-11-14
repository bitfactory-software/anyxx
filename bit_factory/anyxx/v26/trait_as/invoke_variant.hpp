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
consteval std::vector<std::meta::info> extract_trait_as_params(std::meta::info trait_info) {
    auto template_ = template_of(trait_info);
    auto template_arguments = template_arguments_of(trait_info);
    if (template_ == ^^trait_as || template_ == ^^any_) {
        return std::ranges::to<std::vector>(template_arguments | std::views::drop(1));
    } else if (template_ == ^^any) {
        return template_arguments;
    } else {
        std::vector<std::meta::info> args{ template_ };
        args.append_range(template_arguments);
        return args;
    }
}
template <std::meta::info Spec, std::meta::info TraitInfo, typename R, typename V, typename... Args>
constexpr R invoke_variant(V&& v, Args... args) {
    return std::visit(overloads{
        [&] <typename T>([[maybe_unused]] T const& x) {
            [[maybe_unused]] constexpr auto trait_args = std::define_static_array(extract_trait_as_params(TraitInfo));
            using delegate_trait_as = typename[:make_trait_as_for_variant(^^T, trait_args):];
            [[maybe_unused]] constexpr auto target = find_candidate_in<delegate_trait_as, Spec>();
            return delegate_trait_as{ x }.[:target:](args...);
        },[&]<is_any Any>(Any const& x) {
            [[maybe_unused]] constexpr auto target = find_candidate_in<Any, Spec>();
            return x.[:target:](args...);
        }}, std::forward<V>(v));
}

template <std::meta::info Spec, std::meta::info Any, typename V, typename R, typename VoidSelf, typename... Args>
constexpr R invoke_variant_void(VoidSelf self, Args... args) {
    auto typed_self = static_cast<self_const_correct_t<V, VoidSelf>*>(self);
    return invoke_variant<Spec, Any, R>(*typed_self, args...);
}


}  // namespace anyxx
