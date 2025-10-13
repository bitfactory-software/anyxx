#pragma once

namespace anypp {
template <typename... ARGS>
struct type_list {
  // see
  // https://vittorioromeo.info/index/blog/cpp20_lambdas_compiletime_for.html
  static void for_each(auto&& call) { (call.template operator()<ARGS>(), ...); }
};

template <int N, typename... Ts>
using nth_t = typename std::tuple_element<N, std::tuple<Ts...>>::type;

template <typename... Ts>
using first_t = nth_t<0, Ts...>;
}  // namespace anypp
