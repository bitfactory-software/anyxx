#pragma once

#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any/bases.hpp>
#include <bit_factory/v26/any/facade.hpp>
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
    v_table(std::in_place_type_t<Concrete>) {
        set_v_table_members<v_table_t, ^^dyn_self_val_t<Trait, Args...>, ^^dyn_self_cref_t<Trait, Args...>, ^^dyn_self_mutref_t<Trait, Args...>, Concrete, ^^fptrs_t>(this);
    }
};

template <template <typename, typename, typename...> typename Trait, typename V, typename... Args>
v_table<Trait, Args...>* v_table_instance() {
   return anyxx::v_table_instance<v_table<Trait, Args...>, V>();
};

template <typename T, template <typename, typename, typename...> typename Trait, typename... Args>
struct register_trait {
    register_trait() {
        anyxx::bind_v_table_to_meta_data<v_table<Trait, Args...>, T>();
    }
};

template <typename ToVtable, typename FromVTable>
    requires std::derived_from<typename FromVTable::trait_declaration_t, typename ToVtable::trait_declaration_t>
ToVtable* v_table_cast(FromVTable* from) {
    auto void_p = static_cast<void*>(from);
    return static_cast<ToVtable*>(void_p);
}

}
