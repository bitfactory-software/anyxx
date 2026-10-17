#pragma once

#include <array>
#include <bit_factory/anyxx/v26/any/keywords.hpp>
#include <bit_factory/anyxx/v26/any/v_table.hpp>
#include <bit_factory/anyxx/proxies/dyn/shared.hpp>
#include <bit_factory/anyxx/proxies/dyn/val.hpp>
#include <bit_factory/anyxx/v_table_data.hpp>

namespace anyxx {

struct type_info_{
    using type = std::type_info const*;
    template<typename Concrete>
    static std::type_info const* init(auto){ return &typeid(Concrete); }
};
struct is_derived_from_{
    using type = anyxx::is_derived_from_t;
    template<typename Concrete, typename VTable>
    static anyxx::is_derived_from_t init(VTable* ){
        return &is_v_table_derived_from<VTable>;
    }
};
struct meta_data_{
    using type = anyxx::meta_data*;
    template<typename Concrete>
    static auto init(auto){ return nullptr; };
};
struct model_size {
    using type = anyxx::model_size_t;
    template<typename Concrete>
    static auto init(auto){ return anyxx::compute_model_size<Concrete>(); };
};
struct copy_constructor {
    using type = anyxx::copy_constructor_t;
    template<typename Concrete>
    static auto init(auto){ 
        return []([[maybe_unused]] anyxx::mutable_void placement,
                  [[maybe_unused]] anyxx::const_void from) {
            return anyxx::invoke_copy_constructor<Concrete>(placement, from);
        };
    }
};
struct move_constructor {
    using type = anyxx::move_constructor_t;
    template<typename Concrete>
    static auto init(auto){
        return []([[maybe_unused]] anyxx::mutable_void placement,
            [[maybe_unused]] anyxx::mutable_void from) {
            return anyxx::invoke_move_constructor<Concrete>(placement, from);
        };
    }
};
struct delete_ {
    using type = anyxx::delete_t;
    template<typename Concrete>
    static auto init(auto){
        return [](anyxx::mutable_void data) {
            if(data) delete static_cast<Concrete*>(data);
        };
    }
};
struct destructor {
    using type = anyxx::destructor_t;
    template<typename Concrete>
    static auto init(auto){
        return [](anyxx::mutable_void data) {
            std::destroy_at(static_cast<Concrete*>(data));
        };
    }
};

// clang-format off
struct mutable_referenceable {
    template <is_trait Trait, typename Self, typename...>
    struct self_apply {
        using default_proxy_t = anyxx::mutref;
    };
};
struct const_referenceable {
    template <is_trait Trait, typename Self, typename...>
    struct self_apply {
        using default_proxy_t = anyxx::cref;
    };
};

template <is_trait Trait = declaration, typename Self = declaration>
struct deleteable {
    using delete_ [[= v_table_data]] = anyxx::delete_;
    template <is_trait ApplyTrait, typename ApplSelf>
    struct self_apply {
        using default_proxy_t = anyxx::unique;
    };
};

template <is_trait Trait = declaration, typename Self = declaration>
struct moveable {
    using model_size [[= v_table_data]] = anyxx::model_size;
    using move_constructor [[= v_table_data]] = anyxx::move_constructor;
    using destructor [[= v_table_data]] = anyxx::destructor;
    using default_proxy_t = anyxx::val<>;
    template <is_trait ApplyTrait, typename ApplSelf>
    using self_apply = moveable<ApplyTrait, ApplSelf>;
};
    template <is_trait Trait = declaration, typename Self = declaration>
    struct copyable : moveable<Trait, Self> {
        using copy_constructor [[= v_table_data]] = anyxx::copy_constructor;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = copyable<ApplyTrait, ApplSelf>;
    };
template <is_trait Trait = declaration, typename Self = declaration>
struct typed_observeable {
    using type_info_ [[= v_table_data]] = anyxx::type_info_;
    template <is_trait ApplyTrait, typename ApplSelf>
    using self_apply = typed_observeable<ApplyTrait, ApplSelf>;
};
    template <is_trait Trait = declaration, typename Self = declaration>
    struct typed_moveable : typed_observeable<Trait, Self> {
        using model_size [[= v_table_data]] = anyxx::model_size;
        using move_constructor [[= v_table_data]] = anyxx::move_constructor;
        using destructor [[= v_table_data]] = anyxx::destructor;
        using default_proxy_t = anyxx::val<>;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = typed_moveable<ApplyTrait, ApplSelf>;
    };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct typed_copyable : typed_moveable<Trait, Self> {
            using copy_constructor [[= v_table_data]] = anyxx::copy_constructor;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = typed_copyable<ApplyTrait, ApplSelf>;
        };
    template <is_trait Trait = declaration, typename Self = declaration >
    struct dynamic_castable : typed_observeable<Trait, Self> {
        using is_derived_from_ [[= v_table_data]] = anyxx::is_derived_from_;
        using meta_data_ [[= v_table_data]] = anyxx::meta_data_;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = dynamic_castable<ApplyTrait, ApplSelf>;
    };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct dynamic_deletable : dynamic_castable<Trait, Self> {
            using delete_ [[= v_table_data]] = anyxx::delete_;
            using default_proxy_t = anyxx::shared;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = dynamic_deletable<ApplyTrait, ApplSelf>;
        };
            template <is_trait Trait = declaration, typename Self = declaration>
            struct dynamic_smart_ptr : dynamic_deletable<Trait, Self> {
                using model_size [[= v_table_data]] = anyxx::model_size;
                using move_constructor [[= v_table_data]] = anyxx::move_constructor;
                template <is_trait ApplyTrait, typename ApplSelf>
                using self_apply = dynamic_smart_ptr<ApplyTrait, ApplSelf>;
            };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct dynamic_moveable : dynamic_castable<Trait, Self> {
            using model_size [[= v_table_data]] = anyxx::model_size;
            using move_constructor [[= v_table_data]] = anyxx::move_constructor;
            using destructor [[= v_table_data]] = anyxx::destructor;
            using default_proxy_t = anyxx::val<>;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = dynamic_moveable<ApplyTrait, ApplSelf>;
        };
            template <is_trait Trait = declaration, typename Self = declaration>
            struct dynamic_copyable : dynamic_moveable<Trait, Self> {
                using copy_constructor [[= v_table_data]] = anyxx::copy_constructor;
                template <is_trait ApplyTrait, typename ApplSelf>
                using self_apply = dynamic_copyable<ApplyTrait, ApplSelf>;
            };
// clang-format on

}  // namespace anyxx

