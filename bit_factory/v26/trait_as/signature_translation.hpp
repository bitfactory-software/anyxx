#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>

namespace anyxx26 {

template <typename TraitAs, std::meta::info Spec>
consteval std::meta::info translate_trait_as_return_type() {
    constexpr std::meta::info return_type = return_type_of(Spec);
    if constexpr(return_type == ^^declaration&) {
        return ^^TraitAs&;
    } else if constexpr(return_type == ^^declaration) {
        return remove_const(^^TraitAs);
    } else {
        return return_type;
    }
}
template <typename TraitAs, std::meta::info Spec>
using trait_as_return_type_t = [:translate_trait_as_return_type<TraitAs, Spec>():];

template <typename TraitAs, std::meta::info spec, typename Self, typename R>
decltype(auto) forward_trait_as_return(Self&& self, R&& r) {
  constexpr auto return_type = return_type_of(spec);
  if constexpr (return_type == ^^declaration&) {
    return *self_cast<TraitAs>(&self);
  } else if constexpr (return_type == ^^declaration) {
    return TraitAs{std::forward<R>(r)};
  } else if constexpr (return_type == ^^void) {
    return;
  } else {
    return std::forward<R>(r);
  }
}


template <typename TraitAs>
consteval std::meta::info translate_trait_as_param_type(std::meta::info param){
    if(param == ^^declaration const&) {
        return ^^TraitAs const&;
    } else if(param == ^^declaration&) {
        return ^^TraitAs&;
    } else {
        return param;
    }
}

template <typename TraitAs>
consteval auto make_trait_as_params(std::meta::info spec){
  return parameters_of(spec) 
      | std::views::drop(is_static_member(spec) ? 1 : 0)
      | std::views::transform([](std::meta::info param) { return translate_trait_as_param_type<TraitAs>(type_of(param)); });
}

template <typename TraitAs, std::meta::info spec, typename Arg>
decltype(auto) forward_trait_as_param(Arg arg) {
    if constexpr(std::same_as<std::remove_cvref_t<Arg>, std::remove_cvref_t<TraitAs>>) {
        return get_value(arg);
    } else {
        return arg;
    }
}

}  // namespace anyxx26::meta