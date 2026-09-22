#pragma once

#include <bit_factory/v26/dyn.hpp>

namespace anyxx26 {

template <template <is_trait, typename, typename...> typename Base>
struct base {
    template <is_trait Trait, typename Self, typename... Args >  
    using base_t = Base<Trait, Self, Args...>;
};
struct mutable_referenceable {
    template <is_trait Trait, typename Self, typename...>
      struct base_t {
          using default_proxy_t = anyxx::mutref;
      };
};
struct const_referenceable {
    template <is_trait Trait, typename Self, typename...>
    struct base_t {
        using default_proxy_t = anyxx::cref;
    };
};

template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function;
template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function<Trait, Self, Base, R(Args...) const> : Base::template base_t<Trait, Self> {
    R operator()(Args... args) const;
};
template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function<Trait, Self, Base, R(Args...)> : Base::template base_t<Trait, Self> {
    R operator()(Args... args);
};


}