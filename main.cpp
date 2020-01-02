// abstract concept for 32 bit mcu
#include <cstdint> // std::size_t
#include <iostream>
#include <bitset> // для вывода в поток
#include "type_pack.hpp"
#include "registers_set.hpp"



// значение данного типа передаются в функцию, но от него нельзя наследоваться
enum struct Enum1 { _0, _1, _2, _3 };
// поэтому создаётся дополнительный тип, описание свойств,
// в дальнейшем всё равно надо знать как минимум маску
// для определения куда значение записывать
struct Enum1_traits { 
    static constexpr std::size_t mask = 0b111;
};
// нужна функция, связывающая эти 2 типа, используем value based подход
constexpr auto traits(type_identity<Enum1>) { return type_identity<Enum1_traits>{}; }

enum struct Enum2 { _0, _1, _2, _3 };
struct Enum2_traits { 
    static constexpr std::size_t mask = 0b11000;
};
constexpr auto traits(type_identity<Enum2>) { return type_identity<Enum2_traits>{}; }

enum struct Enum3 { _0, _1, _2, _3, _4 };
struct Enum3_traits { 
    static constexpr std::size_t mask = 0b111;
};
constexpr auto traits(type_identity<Enum3>) { return type_identity<Enum3_traits>{}; }

enum struct Enum4 { _0, _1, _2, _3 };
struct Enum4_traits { 
    static constexpr std::size_t mask = 0b1100;
};
constexpr auto traits(type_identity<Enum4>) { return type_identity<Enum4_traits>{}; }

// наследыванием показываем какие перечисления (их свойства) относятся к какому регистру
// быть может здесь надо не наследования применять, а агрегацию, 
// по аналогии с тем как регистры храняться в переферии
struct Register1 : Enum1_traits, Enum2_traits {
    // храним смещение регистра относительно адреса переферии
    static constexpr std::size_t offset = 0x0;
};
struct Register2 : Enum3_traits {
    static constexpr std::size_t offset = sizeof(std::size_t);
};
struct Register3 : Enum4_traits {
    static constexpr std::size_t offset = 0;
};

// сразу надо задуматься о тестировании
// конкретная переферия (тут и далее условно gpioa) должна располaгаться на определённом адресе
// передадим информцию об этом через параметры шаблона
// у класса Address должно быть статическое поле address
template<class Address>
struct GPIO {
    // а параметр шаблона определим из конструктора
    GPIO(Address) {}

    // наследование не очень подошло, поскольку не нашел способа из наследуемых сделать вот это
    // тут показываем с какими регистрами работает переферия
    static constexpr auto registers = type_pack<Register1, Register2>{};

    // у переферии описываем необходимый метод, передавая в свободную функцию
    // список регистров, адрес и пробрасываем аргументы
    template<class...Ts>
    static constexpr void set(Ts...args) {
        ::set(registers, Address::address, args...);
    }
};

// опишем один из типов, предоставляющий адрес
struct GPIOA_address {
    static constexpr std::size_t address = 0x80000000;
};

// чтобы не давать лишний раз пользователю задумываться об адресах,
// можно создать конкретные типы, где адреса уже определены
struct GPIOA : GPIO<GPIOA_address> {
    GPIOA() : GPIO{GPIOA_address{}} {};
};

// пример структуры, которая предоставляет адрес динамически (для тестов)
struct Address {
    static inline std::size_t address;
    template<class Pointer>
    Address(Pointer address_) { address = reinterpret_cast<std::size_t>(address_); }
};

int main() {
    // пример без передачи конкретного адреса
    // закоментированно, потому что при выполнении нельзя писать куда попало (конкретный адрес)
    // auto gpioa = GPIOA{};
    // gpioa.set(Enum1::_1, Enum2::_3, Enum3::_4); // all ok
    // gpioa.set(Enum4::_0);                       // must be compilation error


    // если необходимо передать адрес динамически (для тестов)
    // поскольку адрес динамический, то эту часть можно выполнить
    std::size_t arr[] = {0, 0};
    auto address = Address{arr};
    auto mock_gpioa = GPIO{address};
    // что должна сделать следующая функция:
    // записать значение 1 в первый регистр без смещения + значение 3 со смещением 3 (итого 25)
    // записать значение 4 во второй регистр без смещения
    // итого в первом регистре 0b00011001
    //      во втором регистре 0b00000100
    // https://godbolt.org/z/Rq5XaA копипаст всего понаписаного, видно что в ассемблере нет ничего лишнего
    mock_gpioa.set(Enum1::_1, Enum2::_3, Enum3::_4); // all ok
    // mock_gpioa.set(Enum4::_0);                       // must be compilation error

    // убедимся, что всё записалось
    for (auto v : arr) {
        std::cout << std::bitset<8>(v) << std::endl;
    }

}