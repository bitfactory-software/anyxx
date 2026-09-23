#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec.hpp>
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

//template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
//consteval std::meta::info make_static_facade_call(interface_spec const& spec){
//    std::vector<std::meta::info> types
//    { ^^V
//    , reflect_constant(spec.member)
//    , std::meta::reflect_constant(spec.index)
//    , translate_facade_return_type(return_type_of(spec.member),^^ typename AnyBase::dyn_self_t)
//    };
//    types.append_range(parameters_of(spec.member)
//        | std::views::drop(is_static_member(spec.member) ? 1 : 0)
//        | std::views::transform([](auto p){
//        return translate_v_table_fptr_param_type(^ ^typename AnyBase::dyn_self_cref_t, ^^ typename AnyBase::dyn_self_mutref_t, type_of(p));
//    })
//    );
//    if(is_const_function(spec.member)) {
//        return substitute(^ ^const_dyn_facade_call, types);
//    } else {
//        if(!anyxx::is_const_data<typename AnyBase::proxy_t>) {
//            return substitute(^ ^mutable_dyn_facade_call, types);
//        }
//    }
//    return std::meta::info{};
//}
//
//template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
//consteval std::meta::info static_facade_named_overload_set(overload_set_spec const& spec){
//    std::vector<std::meta::info> overload_set;
//    for(auto overload : spec.specs) {
//        if(auto call = make_static_facade_call<V, Trait, Args...>(overload); call != std::meta::info{}) {
//            overload_set.push_back(call);
//        }
//    }
//    auto overloaded_call_operator = substitute(^^meta::overload, overload_set);
//    return std::meta::data_member_spec(overloaded_call_operator, { .name = spec.name, .no_unique_address = true });
//}
//
//template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
//consteval auto make_static_facade_overloaded_calls() {
//    std::vector<std::meta::info> calls;
//    for(auto const& set : make_overload_sets_specs<Trait, Args...>()) {
//        calls.push_back(reflect_constant(static_facade_named_overload_set<V>(set)));
//    }
//    return calls;
//};


template <template <is_trait, typename> typename Trait, typename V>
consteval std::meta::info make_trait_facade() {
  std::vector<std::meta::info> calls;
//  template for(constexpr auto v_table_spec : define_static_array(get_interface_specs(^^trait_declaration<Trait>))){

  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto m : define_static_array(members_of(^^Trait<model_map, V>, ctx))) {
    if constexpr (has_identifier(m) && is_static_member(m) && is_function(m)) {
      using trait_facade_call_t = trait_facade_call<V, m>;
      constexpr std::meta::info wrapped_meta = ^^trait_facade_call_t;
      auto dms = std::meta::data_member_spec(
          wrapped_meta, {.name = identifier_of(m), .no_unique_address = true});
      calls.push_back(reflect_constant(dms));
    }
  }
  return substitute(^^meta::to_struct, calls);
};

template <typename V, template <is_trait, typename> typename Trait>
class trait_as : public[:make_trait_facade<Trait, V>():] {
  V value_;

 public:
  trait_as(V const& value) : value_(value) {}
};

}  // namespace anyxx26
