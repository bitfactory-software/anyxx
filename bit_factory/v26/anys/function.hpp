#pragma once

#include <bit_factory/v26/any.hpp>

namespace anyxx26 {

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

}