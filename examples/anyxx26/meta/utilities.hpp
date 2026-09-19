#pragma once

#include <meta>
#include <algorithm>
#include <cctype>

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

consteval std::meta::info get_data_member_by_id(std::meta::info in, std::string_view id) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : members_of(in, ctx)) {
        if(has_identifier(m) && identifier_of(m) == id) {
            return m;
        }
    }
    return {};
}
consteval std::string_view function_name_of(std::meta::info in) {
    if(has_identifier(in)) {
        return identifier_of(in);
    } else if(is_operator_function(in)) {
        return anyxx26::meta::enum_to_string(operator_of(in));
    } else {
        return {};
    }
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

consteval bool is_op_spec(std::meta::info spec, std::meta::operators op){
    if (has_identifier(spec) && identifier_of(spec) == anyxx26::meta::enum_to_string(op)){
        return true;
    }
    if (is_operator_function(spec) && operator_of(spec) == op){
        return true;
    }
    return false;
}

consteval bool is_op_parentheses_spec(std::meta::info spec){
    return is_op_spec(spec, std::meta::op_parentheses);
}

consteval std::meta::info get_member_by_function_name(std::meta::info in, std::string_view id) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : members_of(in, ctx)) {
        if(function_name_of(m) == id) {
            return m;
        }
    }
    return {};
}

}  // namespace anyxx26::meta