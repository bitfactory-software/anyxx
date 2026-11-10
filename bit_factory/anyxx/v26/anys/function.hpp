#pragma once

#include <bit_factory/anyxx/v26/any.hpp>

namespace anyxx {

template <typename Self, typename Arg0, typename... Args>
struct function;
template <typename Self, typename R, typename... Args>
struct function<Self, R(Args...) const> : copyable<Self> {
    R operator()(Args... args) const;
};
template <typename Self, typename R, typename... Args>
struct function<Self, R(Args...)> : copyable<Self> {
    R operator()(Args... args);
};

template <typename Self, typename Arg0, typename... Args>
struct move_only_function;
template <typename Self, typename R, typename... Args>
struct move_only_function<Self, R(Args...) const> : moveable<Self> {
    R operator()(Args... args) const;
};
template <typename Self, typename R, typename... Args>
struct move_only_function<Self, R(Args...)> : moveable<Self> {
    R operator()(Args... args);
};

template <typename Self, typename Arg0, typename... Args>
struct function_ref;
template <typename Self, typename R, typename... Args>
struct function_ref<Self, R(Args...) const> {
    R operator()(Args... args) const;
    using default_proxy_t = anyxx::cref;
};
template <typename Self, typename R, typename... Args>
struct function_ref<Self, R(Args...)> {
    R operator()(Args... args);
    using default_proxy_t = anyxx::mutref;
};

}