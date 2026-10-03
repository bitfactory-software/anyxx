#pragma once

#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/any/signature_translation.hpp>
#include <bit_factory/v26/any/set_v_table_members.hpp>
#include <bit_factory/v26/any/make_v_table_members_type.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>

namespace anyxx26 {

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table
    : [: make_v_table_members_type<Trait, Args...>():] {
    using v_table_t = v_table;
	using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
    using fptrs_t = [:make_v_table_members_type<Trait, Args...>():];
    template <typename Concrete>
    constexpr v_table(std::in_place_type_t<Concrete>) {
        set_v_table_members<v_table_t, ^^dyn_self_val_t<Trait, Args...>, ^^dyn_self_cref_t<Trait, Args...>, ^^dyn_self_mutref_t<Trait, Args...>, Concrete, ^^fptrs_t>(this);
    }
};

template <template <typename, typename, typename...> typename Trait, typename V, typename... Args>
constexpr v_table<Trait, Args...>* v_table_instance() {
   return anyxx::v_table_instance<v_table<Trait, Args...>, V>();
};

template <typename T, template <typename, typename, typename...> typename Trait, typename... Args>
struct register_trait {
    constexpr register_trait() {
        anyxx::bind_v_table_to_meta_data<v_table<Trait, Args...>, T>();
    }
};

template <typename ToVtable, typename FromVTable>
    requires anyxx::is_any_derived_from_v<FromVTable, ToVtable>
constexpr ToVtable* v_table_cast(FromVTable* from) {
    auto void_p = static_cast<void*>(from);
    return static_cast<ToVtable*>(void_p);
}

template <typename ToVtable, typename FromVTable>
    requires (!anyxx::is_any<ToVtable> && anyxx::is_any_derived_from_v<ToVtable, FromVTable>)
constexpr ToVtable* unchecked_v_table_downcast_to(FromVTable* from) {
    auto void_p = static_cast<void*>(from);
    return static_cast<ToVtable*>(void_p);
}


consteval std::meta::info v_table_of_trait(std::meta::info declaration_trait) {
    std::vector<std::meta::info> v_table_template_args;
    v_table_template_args.push_back(template_of(declaration_trait));
    v_table_template_args.append_range(template_arguments_of(declaration_trait) | std::views::drop(2));
    return substitute(^^v_table, v_table_template_args);
}

template<typename VTable>
constexpr bool is_v_table_derived_from(const std::type_info& from) {
    if (from == typeid(VTable)) {
        return true;
    }
    if constexpr(constexpr auto base = meta::get_type_of_single_public_base(^^typename VTable::trait_declaration_t); base != std::meta::info{}) {
        using base_v_table_t = [:v_table_of_trait(base):];
        return is_v_table_derived_from<base_v_table_t>(from);
    }
    return false;
}

}  // namespace anyxx26