#pragma once

#include "type_identity.hpp"
#include "metafn_wrapper.hpp"
#include <type_traits>

template <class...Ts>
struct type_pack{};

using empty_pack = type_pack<>;

template <class...Ts>
constexpr auto make_type_pack(type_identity<Ts>...) {
    return type_pack<Ts...>{};
}

template <class...Ts>
constexpr auto size(type_pack<Ts...>) { return sizeof...(Ts); }

template <class...Ts>
constexpr auto empty(type_pack<Ts...> v) { return size(v) == 0; }

// dont need, just example
template <class T, class...Ts>
constexpr auto head(type_pack<T, Ts...>) { return type_identity<T>{}; }

// dont need, just example
template <class T, class...Ts>
constexpr auto tail(type_pack<T, Ts...>) { return type_pack<Ts...>{}; }

template <class...Ts, class...Us>
constexpr auto operator== (type_pack<Ts...>, type_pack<Us...>) { return false; }

template <class...Ts>
constexpr auto operator== (type_pack<Ts...>, type_pack<Ts...>) { return true; }

template <class...Ts, class...Us>
constexpr auto operator!= (type_pack<Ts...>, type_pack<Us...>) { return true; }

template <class...Ts>
constexpr auto operator!= (type_pack<Ts...>, type_pack<Ts...>) { return false; }

// dont need, just example
template <class T, class...Ts>
constexpr auto push_front(type_pack<Ts...>, type_identity<T>) { return type_pack<T, Ts...>{}; }

template <class T, class...Ts>
constexpr auto containts(type_pack<Ts...>, type_identity<T>) { 
    return (std::is_same_v<T, Ts> or ...); 
}

template <class T, class...Ts>
constexpr auto find(type_pack<Ts...> pack, type_identity<T>) { 
    bool arsame[] = { std::is_same_v<T,Ts>... };
    // better use std::find in c++20
    std::size_t i {0};
    for (auto same : arsame) {
        if (same)
            break;
        i++;
    }
    return i;
}

template <class F, class...Ts>
constexpr auto find_if(type_pack<Ts...> pack, F f) { 
    bool arsame[] = { f(type_identity<Ts>{})... };
    // better use std::find in c++20
    std::size_t i {0};
    for (auto same : arsame) {
        if (same)
            break;
        i++;
    }
    return i;
}

template <class F, class...Ts>
constexpr auto all_of(type_pack<Ts...> pack, F f) {
    return (f(type_identity<Ts>{}) and ...);
}

template <class F, class...Ts>
constexpr auto any_of(type_pack<Ts...> pack, F f) {
    return (f(type_identity<Ts>{}) or ...);
}

template <class F, class...Ts>
constexpr auto none_of(type_pack<Ts...> pack, F f) {
    return not any_of(pack, f);
}



namespace type_pack_test {
    struct A {};
    struct B : A {};
    struct C : B {};
    struct D {};
    constexpr auto pack = type_pack<int, bool, char>{};

    static_assert(size(pack) == 3);
    static_assert(not empty(pack));
    static_assert(empty(empty_pack{}));

    static_assert(std::is_same_v<decltype(head(pack))::type,int>);
    static_assert(std::is_same_v<decltype(tail(pack)), decltype(type_pack<bool,char>{})>);

    static_assert(pack == type_pack<int, bool, char>{});
    static_assert(pack != type_pack<int, bool, int>{});
    static_assert(tail(pack) == type_pack<bool, char>{});

    static_assert(push_front(pack, type_identity<bool>{}) == type_pack<bool, int, bool, char>{});

    static_assert(containts(pack, type_identity<bool>{}));
    static_assert(not containts(pack, type_identity<const int>{}));

    static_assert(find(pack, type_identity<bool>{}) == 1);
    static_assert(find(pack, type_identity<int*>{}) == 3);

    static_assert(find_if(pack, value_fn<std::is_integral>{}) == 0);
    static_assert(find_if(pack, value_fn<std::is_pointer>{}) == 3);

    static_assert(all_of(pack, value_fn<std::is_integral>{}));
    static_assert(not all_of(type_pack<int, int*>{}, value_fn<std::is_integral>{}));
    static_assert(all_of(type_pack<B, C>{}, value_fn<std::is_base_of, A>{}));

    static_assert(not any_of(pack, value_fn<std::is_pointer>{}));
    static_assert(any_of(type_pack<int, int*>{}, value_fn<std::is_pointer>{}));

    static_assert(none_of(pack, value_fn<std::is_pointer>{}));
    static_assert(not none_of(type_pack<int, int*>{}, value_fn<std::is_pointer>{}));
}