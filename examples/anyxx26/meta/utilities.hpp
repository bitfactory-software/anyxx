#pragma once

#include <meta>

namespace anyxx26::meta {

template <std::meta::info... Ms>
struct outer {
  struct inner;
  consteval {
    define_aggregate(^^inner, {
                                  Ms...});
  }
};
template <std::meta::info... Ms>
using to_struct = outer<Ms...>::inner;

template <std::meta::info Struct, auto id>
consteval std::meta::info get_member_by_id() {
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m :
        define_static_array(members_of(Struct, ctx))) {
        if constexpr(has_identifier(m) &&
            identifier_of(m) == std::string_view{id}) {
            return m;
        }
    }
    return {};
}

template <std::meta::info Struct, std::meta::info Other>
consteval std::meta::info get_member() {
  return get_member_by_id<Struct, define_static_string(identifier_of(Other))>();
}

template <std::meta::info Struct>
consteval std::meta::info get_single_public_base() {
  static_assert(is_type(Struct));
  constexpr auto ctx = std::meta::access_context::current();
  constexpr auto bases = define_static_array(bases_of(Struct, ctx));
  if (bases.size() > 1) {
    throw std::logic_error("Struct can have at most one public base");
  }
  if (bases.size() == 0) {
    return {};
  }
  return bases[0];
}

template<typename E, bool Enumerable = std::meta::is_enumerable_type(^^ E)>
    requires std::is_enum_v<E>
constexpr std::string_view enum_to_string(E value) {
    if constexpr(Enumerable)
        template for(constexpr auto e :
            std::define_static_array(std::meta::enumerators_of(^^ E)))
        if(value == [:e:])
            return std::meta::identifier_of(e);

    return "<unnamed>";
}

template<std::meta::info spec>
constexpr bool is_op_parentheses_spec(){
    if constexpr(has_identifier(spec) && identifier_of(spec) == anyxx26::meta::enum_to_string(std::meta::op_parentheses)){
        return true;
    }
    if constexpr(is_operator_function(spec) && operator_of(spec) == std::meta::op_parentheses){
        return true;
    }
    return false;
}

}  // namespace anyxx26::meta