#pragma once

#include <meta>

namespace anyxx26::meta {

template <std::meta::info ...Ms>
struct outer {
    struct inner;
    consteval {
        define_aggregate(^^inner, { Ms... });
    }
};
template <std::meta::info ...Ms>
using to_struct = outer<Ms...>::inner;

}
