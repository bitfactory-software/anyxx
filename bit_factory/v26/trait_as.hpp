#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec.hpp>
#include <bit_factory/v26/trait_translation/get_implementation_member.hpp>
#include <meta>

namespace anyxx26 {

template <typename V, std::meta::info Target>
struct trait_facade_call {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    const V* pvalue = reinterpret_cast<const V*>(this);
    return [:Target:](*pvalue, std::forward<Args>(args)...);
  }
};

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_static_facade_call(interface_spec const& spec){
    auto args = std::define_static_array(template_arguments_of(spec.declaration_trait) | std::views::drop(2));
    auto implemenation_member = find_function_impl(spec.member, ^^Trait, ^^V, args);
    if(is_const_function(spec.member)) {
        return substitute(^^trait_facade_call, {^^V const, reflect_constant(implemenation_member)});
    } else {
        return substitute(^^trait_facade_call, {^^V, reflect_constant(implemenation_member)});
    }
}

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info static_facade_named_overload_set(overload_set_spec const& spec){
    std::vector<std::meta::info> overload_set;
    for(auto overload : spec.specs) {
        if(auto call = make_static_facade_call<V, Trait, Args...>(overload); call != std::meta::info{}) {
            overload_set.push_back(call);
        }
    }
    auto overloaded_call_operator = substitute(^^meta::overload, overload_set);
    return std::meta::data_member_spec(overloaded_call_operator, { .name = spec.name, .no_unique_address = true });
}

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval auto make_static_facade_overloaded_calls() {
    std::vector<std::meta::info> calls;
    for(auto const& set : make_overload_sets_specs<Trait, Args...>()) {
        calls.push_back(reflect_constant(static_facade_named_overload_set<V, Trait, Args...>(set)));
    }
    return calls;
};

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_trait_facade() {
    return substitute(^^meta::to_struct, make_static_facade_overloaded_calls<V, Trait, Args...>());
};

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
class trait_as : public[:make_trait_facade<V, Trait, Args...>():] {
  V value_;

 public:
  trait_as(V const& value) : value_(value) {}
};

}  // namespace anyxx26
