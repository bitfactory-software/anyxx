#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

struct type_info_{
    using type = std::type_info const*;
    template<typename Concrete>
    static std::type_info const* init(auto){ return &typeid(Concrete); }
};
struct is_derived_from_{
    using type = anyxx::is_derived_from_t;
    template<typename Concrete, typename VTable>
    static anyxx::is_derived_from_t init(VTable* ){
        return +[]([[maybe_unused]] const std::type_info& from) {
            return false;
        //return VTable::static_is_derived_from(
        //    from);  // not yet implemented! must build a list of base classes in
        //            // the vtable and check if from is in that list
        };
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

struct dispatch_table {
    using type = anyxx::dispatch_table_t*;
    template<typename Concrete, typename VTable>
    static auto init(VTable*){
        return anyxx::dispatch_table_instance<VTable, Concrete>();
    }
};

// clang-format off
//template <template <is_trait, typename, typename...> typename Base>
//struct base {
//    template <is_trait Trait, typename Self, typename... Args >
//    using self_apply = Base<Trait, Self, Args...>;
//};
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
struct moveable {
    using model_size [[= v_table_data]] = anyxx26::model_size;
    using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
    using destructor [[= v_table_data]] = anyxx26::destructor;
    using default_proxy_t = anyxx::val<>;
    template <is_trait ApplyTrait, typename ApplSelf>
    using self_apply = moveable<ApplyTrait, ApplSelf>;
};
    template <is_trait Trait = declaration, typename Self = declaration>
    struct copyable : moveable<Trait, Self> {
        using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = copyable<ApplyTrait, ApplSelf>;
    };
template <is_trait Trait = declaration, typename Self = declaration>
struct save_observable {
    using type_info_ [[= v_table_data]] = anyxx26::type_info_;
    template <is_trait ApplyTrait, typename ApplSelf>
    using self_apply = save_observable<ApplyTrait, ApplSelf>;
};
    template <is_trait Trait = declaration, typename Self = declaration>
    struct save_moveable : save_observable<Trait, Self> {
        using model_size [[= v_table_data]] = anyxx26::model_size;
        using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
        using destructor [[= v_table_data]] = anyxx26::destructor;
        using default_proxy_t = anyxx::val<>;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = save_moveable<ApplyTrait, ApplSelf>;
    };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct save_copyable : save_moveable<Trait, Self> {
            using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = save_copyable<ApplyTrait, ApplSelf>;
        };
    template <is_trait Trait = declaration, typename Self = declaration >
    struct dynamic_castable : save_observable<Trait, Self> {
        using is_derived_from_ [[= v_table_data]] = anyxx26::is_derived_from_;
        using meta_data_ [[= v_table_data]] = anyxx26::meta_data_;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = dynamic_castable<ApplyTrait, ApplSelf>;
    };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct dynamic_deletable : dynamic_castable<Trait, Self> {
            using delete_ [[= v_table_data]] = anyxx26::delete_;
            using default_proxy_t = anyxx::shared;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = dynamic_deletable<ApplyTrait, ApplSelf>;
        };
            template <is_trait Trait = declaration, typename Self = declaration>
            struct dynamic_smart_ptr : dynamic_deletable<Trait, Self> {
                using model_size [[= v_table_data]] = anyxx26::model_size;
                using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
                template <is_trait ApplyTrait, typename ApplSelf>
                using self_apply = dynamic_smart_ptr<ApplyTrait, ApplSelf>;
            };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct dynamic_moveable : dynamic_castable<Trait, Self> {
            using model_size [[= v_table_data]] = anyxx26::model_size;
            using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
            using destructor [[= v_table_data]] = anyxx26::destructor;
            using default_proxy_t = anyxx::val<>;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = dynamic_moveable<ApplyTrait, ApplSelf>;
        };
            template <is_trait Trait = declaration, typename Self = declaration>
            struct dynamic_copyable : dynamic_moveable<Trait, Self> {
                using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
                template <is_trait ApplyTrait, typename ApplSelf>
                using self_apply = dynamic_copyable<ApplyTrait, ApplSelf>;
            };
// clang-format on

}  // namespace anyxx26

