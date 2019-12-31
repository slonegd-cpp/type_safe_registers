// value based обёртка над метафункциями
#pragma once

#include "type_identity.hpp"

template <template <class...> class F, class...Ts>
struct value_fn {
    template <class...Us>
    constexpr auto operator()(type_identity<Us>...) {
        return F<Ts...,Us...>::value;
    }
};

template <template <class...> class F>
constexpr auto value_fn_v = value_fn<F>{};

template <template <class...> class F>
struct type_fn {
    template <class...Ts>
    constexpr auto operator()(type_identity<Ts>...) {
        return type_identity<typename F<Ts...>::type>{};
    }
};

template <template <class...> class F>
constexpr auto type_fn_v = type_fn<F>{};

