#pragma once

#include <type_traits>
#include "type_pack.hpp"

// по маске определяем насколько нужно сдвинуть значение
constexpr auto shift(std::size_t mask) {
    return __builtin_ffs(mask) - 1;
}

static_assert(shift(24) == 3);

// определяем значение в регистре для конкретного аргумента
template<class Register, class Arg>
constexpr std::size_t arg_value(type_identity<Register>, Arg arg) {
    constexpr auto arg_traits = traits(type_identity<Arg>{});
    // нужен тип со свойствами, чтобы достать маску
    using Arg_traits = typename decltype(arg_traits)::type;
    constexpr auto arg_shift = shift(Arg_traits::mask);
    // значение вычисляется только, если аргумент соотвествует регистру
    return std::is_base_of_v<Arg_traits, Register> ? (static_cast<std::size_t>(arg) << arg_shift) : 0;
}

// определяем значение конкретного регистра по всем аргументам
template<class Register, class...Args>
constexpr std::size_t register_value(type_identity<Register> reg, Args...args) {
    std::size_t value {0};
    // бинарные опреаторы недопустимы в выражениях развёртки, поэтому делаем лямбду-обёртку
    auto change = [](auto& value, auto arg_value) {
        value |= arg_value;
    };
    // цикл по всем аргументам, с записью результата в переменную
    (change(value, arg_value(reg, args)), ...);
    return value;
}

// непосредственно запись в регистр, нужны только адрес и значение
void write(std::size_t address, std::size_t v) {
    *reinterpret_cast<std::size_t*>(address) |= v;
}

// дополнительный парамет - адрес переферии, относительно которого нужно 
// будет устанавливать значения из перечислений
template<class...Registers, class...Args>
constexpr void set(type_pack<Registers...> registers, std::size_t address, Args...args) {
    // вся магия будет тут
    
    // из аргументов достаём их свойства и упаковываем, используя value based подход
    constexpr auto traits_pack = make_type_pack(traits(type_identity<Args>{})...);
    // и теперь можно проверить все ли свойства аргументов являются базовыми для заданной переферии
    static_assert(all_of(traits_pack, [](auto value){
        using value_type = typename decltype(value)::type;
        return (std::is_base_of<value_type, Registers>::value || ...);
    }), "one of arguments in set method don`t belong to periph type");


    // определяем значения в каждом регситре
    std::size_t values[] = {register_value(type_identity<Registers>{}, args...)...};

    // запись значений в регистры
    auto i {0};
    (write(Registers::offset + address, values[i++]), ...);
};