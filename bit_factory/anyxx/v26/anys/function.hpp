#pragma once

#include <bit_factory/anyxx/v26/any.hpp>

namespace anyxx {

template <is_trait Trait, typename Self, typename Arg0, typename... Args>
struct function;

template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function<Trait, Self, Base, R(Args...) const> : Base::template self_apply<Trait, Self> {
    R operator()(Args... args) const;
};
template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function<Trait, Self, Base, R(Args...)> : Base::template self_apply<Trait, Self> {
    R operator()(Args... args);
};
template <is_trait Trait, typename Self, typename R, typename... Args>
struct function<Trait, Self, R(Args...) const> : copyable<Trait, Self> {
    R operator()(Args... args) const;
};
template <is_trait Trait, typename Self, typename R, typename... Args>
struct function<Trait, Self, R(Args...)> : copyable<Trait, Self> {
    R operator()(Args... args);
};

template <is_trait Trait, typename Self, typename Arg0, typename... Args>
struct function_ref;
template <is_trait Trait, typename Self, typename R, typename... Args>
struct function_ref<Trait, Self, R(Args...) const> {
    R operator()(Args... args) const;
    using default_proxy_t = anyxx::cref;
};
template <is_trait Trait, typename Self, typename R, typename... Args>
struct function_ref<Trait, Self, R(Args...)> {
    R operator()(Args... args);
    using default_proxy_t = anyxx::mutref;
};

}