#pragma once

template <class T>
struct type_identity { using type = T; };

template <class T>
using type_identity_t = typename type_identity<T>::type;

template <class T, class U>
constexpr auto operator== (type_identity<T>, type_identity<U>) { return false; }

template <class T>
constexpr auto operator== (type_identity<T>, type_identity<T>) { return true; }

template <class T, class U>
constexpr auto operator!= (type_identity<T>, type_identity<U>) { return true; }

template <class T>
constexpr auto operator!= (type_identity<T>, type_identity<T>) { return false; }


namespace type_identity_test {
    template<class T>
    constexpr auto foo(T, T) { return true; }
    
    template<class T>
    constexpr auto bar(T, type_identity_t<T>) { return true; }

    // static_assert(foo(4.2, 0)); // error, deduced conflicting types for parameter ‘T’ (‘double’ and ‘int’)
    static_assert(bar(4.2, 0));

    static_assert(type_identity<int>{} == type_identity<int>{});
    static_assert(type_identity<int>{} != type_identity<char>{});
}