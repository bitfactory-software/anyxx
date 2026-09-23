#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>

namespace anyxx26 {

struct v_table_spec {
  std::meta::info declaration_trait;
  std::meta::info member;
  std::size_t index;
};

consteval bool is_v_table_data(v_table_spec spec) {
    return has_identifier(spec.member) && is_type(spec.member) && annotations_of_with_type(spec.member, ^^v_table_data_t).size() > 0;
}
consteval bool is_function(v_table_spec spec) {
    return has_identifier(spec.member) && is_function(spec.member);
}
consteval bool is_operator(v_table_spec spec) {
    return is_user_declared(spec.member) && is_operator_function(spec.member);
}
consteval bool is_function_or_operator(v_table_spec spec) {
    return is_function(spec) || is_operator(spec);
}

consteval std::string v_table_name_of(std::meta::info member, std::size_t index) {
    std::array<char,10> index_chars;
    auto r = std::to_chars(index_chars.begin(), index_chars.end(), index);
    return std::string{meta::function_name_of(member)} + std::string_view(index_chars.begin(), r.ptr - index_chars.begin());
}

consteval std::string v_table_name_of(v_table_spec spec) {
    if (is_v_table_data(spec)) {
        return std::string{ identifier_of(spec.member) };
    } else {
        return v_table_name_of(spec.member, spec.index);
    }
}

consteval std::vector<v_table_spec> get_v_table_specs(std::meta::info declaration_trait) {
    if (declaration_trait == std::meta::info{}) {
        return {};
    } else {
        declaration_trait = dealias(declaration_trait);
        auto specs = get_v_table_specs(meta::get_type_of_single_public_base(declaration_trait));

        constexpr auto ctx = std::meta::access_context::current();
        for(auto m : members_of(declaration_trait, ctx)) {
            if (has_identifier(m) && is_type(m) && annotations_of_with_type(m, ^^v_table_data_t).size() > 0) {
                specs.push_back({declaration_trait, m, specs.size()});
            } else if (has_identifier(m) && is_function(m)) {
                specs.push_back({declaration_trait, m, specs.size()});
            } else if (is_user_declared(m) && is_operator_function(m)) {
                specs.push_back({declaration_trait, m, specs.size()});
            }
        }
        return specs;
    }
}

}  // namespace anyxx26
