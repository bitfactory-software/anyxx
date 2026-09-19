#pragma once

#include <meta>
#include <algorithm>
#include <examples/anyxx26/dyn/keywords.hpp>
#include <examples/anyxx26/meta/utilities.hpp>

namespace anyxx26 {

consteval std::string decorated_name_of(std::meta::info in) {
    std::string name{ meta::function_name_of(in) };
    //if(is_function(in) || is_operator_function(in)) {
    //    for(auto p : parameters_of(in) | std::views::drop(is_static_member(in) ? 1 : 0)) {
    //        name += display_string_of(type_of(p));
    //    }
    //}
    //const auto r = std::ranges::remove_if(name, [](char c) { return !(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') && !(c >= '0' && c <= '9') && c != '_'; });
    //name.erase(r.begin(), r.end());
    return name;
}
consteval std::meta::info get_member_by_decorated_name(std::meta::info in, std::string_view id) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : members_of(in, ctx)) {
        if(decorated_name_of(m) == id) {
            return m;
        }
    }
    return {};
}

consteval std::meta::info get_member(std::meta::info in, std::meta::info other_member) {
    return get_member_by_decorated_name(in, decorated_name_of(other_member));
}

}  // namespace anyxx26::meta