#pragma once

#include <meta>
#include <print>
#include <examples/anyxx26/meta/utilities.hpp>

namespace anyxx26::meta {

template <typename T>
void print_members(std::string const& indent = {}) {
  constexpr auto type_info = ^^T;
  std::println("{}{}", indent, display_string_of(type_info));
  std::println("{} is_function = {}", indent, is_function(type_info));
  std::println("{} is_type(r) = {}", indent, is_type(type_info));
  std::println("{} is_pointer_type = {}", indent, is_pointer_type(type_info));
  if constexpr(is_pointer_type(type_info)) {
      std::println("{} is_function_type(remove_pointer(...)) = {}", indent, is_function_type(remove_pointer(type_info)));
  }
  std::println("{} is_class_type = {}", indent, is_class_type(type_info));
  std::println("{} is_function_type = {}", indent, is_function_type(type_info));
  std::println("{} is_function_template = {}", indent, is_function_template(type_info));
  std::println("{} is_operator_function_template = {}", indent, is_operator_function_template(type_info));

  if constexpr(is_class_type(^^T)) {
      constexpr auto ctx = std::meta::access_context::unchecked();
      constexpr static auto members =
          std::define_static_array(members_of(type_info, ctx));
      template for(constexpr std::meta::info m : members) {
          if constexpr(is_user_provided(m)) {
              if constexpr(is_operator_function(m)) {
                  constexpr auto op = operator_of(m);
                  std::println("{}{} \"{}\":{}", indent, symbol_of(op), enum_to_string(op), std::meta::display_string_of(m));
              } else {
                  std::println("{}{}", indent, std::meta::display_string_of(m));
              }
              if constexpr(is_op_parentheses_spec<m>()) {
                  std::println("{} ^___ is_op_parentheses_spec", indent);
              }
          }

      }
      constexpr static auto bases =
          std::define_static_array(bases_of(type_info, ctx));
      template for(constexpr std::meta::info b : bases) {
          print_members<typename[:type_of(b):]>(indent + "  ");
      }
  }
}

}  // namespace anyxx26::meta