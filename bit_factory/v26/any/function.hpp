#pragma once

#include <bit_factory/v26/dyn.hpp>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename R, typename... Args>
struct const_copyable_function : copyable<Trait, Self> {
    R operator()(Args... args) const;
};

template <typename R, typename... Args>
struct copyable_function;
template <typename R, typename... Args>
struct copyable_function<R(Args...) const>
    : dyn<const_copyable_function, anyxx::val<>, int, int> {
    using dyn::dyn;
};

}