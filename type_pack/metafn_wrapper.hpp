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

template <template <class...> class F, class...Ts>
struct type_fn {
    template <class...Us>
    constexpr auto operator()(type_identity<Us>...) {
        return type_identity<typename F<Ts...,Us...>::type>{};
    }
};


