#pragma once

#include <meta>

namespace anyxx26::meta {

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

template <std::meta::info... Ms>
struct outer {
  struct inner;
  consteval {
    define_aggregate(^^inner, { Ms...});
  }
};
template <std::meta::info... Ms>
using to_struct = outer<Ms...>::inner;

consteval std::meta::info get_member_by_id(std::meta::info in, auto id) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : members_of(in, ctx)) {
        if (has_identifier(m) && identifier_of(m) == std::string_view{id}) {
            return m;
        }
        if (is_operator_function(m) && anyxx26::meta::enum_to_string(operator_of(m)) == std::string_view{ id }) {
            return m;
        }
    }
    return {};
}

consteval std::meta::info get_member(std::meta::info in, std::meta::info other_member) {
    if (has_identifier(other_member)) {
        return get_member_by_id(in, define_static_string(identifier_of(other_member)));
    } 
    if (is_operator_function(other_member)) {
      return get_member_by_id(in, define_static_string(anyxx26::meta::enum_to_string(operator_of(other_member))));
    }
    return {};
}

template <std::meta::info Struct>
consteval std::meta::info get_type_of_single_public_base() {
  static_assert(is_type(Struct));
  constexpr auto ctx = std::meta::access_context::current();
  constexpr auto bases = define_static_array(bases_of(Struct, ctx));
  if (bases.size() > 1) {
    throw std::logic_error("Struct can have at most one public base");
  }
  if (bases.size() == 0) {
    return {};
  }
  return type_of(bases[0]);
}

template<std::meta::info spec, std::meta::operators op>
constexpr bool is_op_spec(){
    if constexpr(has_identifier(spec) && identifier_of(spec) == anyxx26::meta::enum_to_string(op)){
        return true;
    }
    if constexpr(is_operator_function(spec) && operator_of(spec) == op){
        return true;
    }
    return false;
}

template<std::meta::info spec>
constexpr bool is_op_parentheses_spec(){
    return is_op_spec<spec, std::meta::op_parentheses>();
}

}  // namespace anyxx26::meta