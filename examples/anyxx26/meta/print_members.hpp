#pragma once

#include <meta>
#include <print>

namespace anyxx26::meta {

template <typename T>
void print_members(std::string const& indent = {}) {
  std::println("{}{}", indent, std::meta::display_string_of(^^T));
  constexpr auto ctx = std::meta::access_context::current();
  constexpr auto type_info = ^^T;
  constexpr static auto members =
      std::define_static_array(members_of(type_info, ctx));
  template for (constexpr std::meta::info m : members) {
    if constexpr (has_identifier(m)) {
      std::println("{}{}", indent, std::meta::display_string_of(m));
    }
  }
  constexpr static auto bases =
      std::define_static_array(bases_of(type_info, ctx));
  template for (constexpr std::meta::info b : bases) {
    print_members<typename [:type_of(b):]>(indent + "  ");
  }
}

}  // namespace anyxx26::meta