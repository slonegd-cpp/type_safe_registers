// abstract concept for 32 bit mcu
#include <cstdint> // std::size_t
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
constexpr auto traits(Enum1) { return type_identity<Enum1_traits>{}; }

enum struct Enum2 { _0, _1, _2, _3 };
struct Enum2_traits { 
    static constexpr std::size_t mask = 0b1100;
};
constexpr auto traits(Enum2) { return type_identity<Enum2_traits>{}; }

enum struct Enum3 { _0, _1, _2, _3, _4 };
struct Enum3_traits { 
    static constexpr std::size_t mask = 0b1100;
};
constexpr auto traits(Enum3) { return type_identity<Enum3_traits>{}; }

enum struct Enum4 { _0, _1, _2, _3 };
struct Enum4_traits { 
    static constexpr std::size_t mask = 0b1100;
};
constexpr auto traits(Enum4) { return type_identity<Enum4_traits>{}; }

// наследыванием показываем какие перечисления (их свойства) относятся к какому регистру
// быть может здесь надо не наследования применять, а агрегацию, 
// по аналогии с тем как регистры храняться в переферии
struct Register1 : Enum1_traits, Enum2_traits {
    // храним смещение регистра относительно адреса переферии
    static constexpr std::size_t offset = 0x0;
};
struct Register2 : Enum3_traits {
    static constexpr std::size_t offset = 0x4;
};
struct Register3 : Enum4_traits {
    static constexpr std::size_t offset = 0x8;
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
    auto gpioa = GPIOA{};
    gpioa.set(Enum1::_1, Enum2::_3, Enum3::_4); // all ok
    // gpioa.set(Enum4::_0);                       // must be compilation error


    // если необходимо передать адрес динамически (для тестов), тип пока int
    auto address = Address{new(int)};
    auto mock_gpioa = GPIO{address};
    mock_gpioa.set(Enum1::_1, Enum2::_3, Enum3::_4); // all ok
    // mock_gpioa.set(Enum4::_0);                       // must be compilation error
}