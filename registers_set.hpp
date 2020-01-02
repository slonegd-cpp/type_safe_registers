#pragma once

#include <type_traits>
#include "type_pack.hpp"

template<class T, class...Deriveds>
struct is_base_for {
    static constexpr auto value = (std::is_base_of<T, Deriveds>::value || ...);
};

// дополнительный парамет - адрес переферии, относительно которого нужно 
// будет устанавливать значения из перечислений
template<class...Registers, class...Args>
constexpr void set(type_pack<Registers...> registers, std::size_t address, Args...args) {
    // вся магия будет тут
    
    // из аргументов достаём их свойства и упаковываем, используя value based подход
    auto traits_pack = make_type_pack(traits(args)...);
    // и теперь можно проверить все ли свойства аргументов являются базовыми для заданной переферии
    static_assert(all_of(traits_pack, value_first_fn<is_base_for, Registers...>{}), "one of arguments in set method don`t belong to periph type");
};