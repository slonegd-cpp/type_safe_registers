// abstract concept for 32 bit mcu
#include <cstdint> // std::size_t
#include <type_traits>
#include "type_pack.hpp"



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


// need to change Derived and Base for work with all_of function of type_pack
template <class Derived, class Base>
struct is_base_for {
    static constexpr auto value = std::is_base_of<Base, Derived>::value;
};

// дополнительный парамет - адрес переферии, относительно которого нужно 
// будет устанавливать значения из перечислений
template<class Periph, class...Ts>
constexpr void set(Periph periph, std::size_t address, Ts...args) {
    // вся магия будет тут
    
    // из аргументов достаём их свойства и упаковываем, используя value based подход
    auto traits_pack = make_type_pack(traits(args)...);
    // и теперь можно проверить все ли свойства аргументов являются базовыми для заданной переферии
    static_assert(all_of(traits_pack, value_fn<is_base_for, Periph>{}), "one of arguments in set method don`t belong to periph type");
};


// наследыванием показываем какие регистры относятся к какой переферии
struct Periph1 : Register1, Register2 {};
struct Periph2 : Register3 {};

// сразу надо задуматься о тестировании
// конкретная переферия (тут и далее условно gpioa) должна располaгаться на определённом адресе
// передадим информцию об этом через параметры шаблона
// у класса Address должно быть статическое поле address
template<class Address>
// наследоваться необходимо от класса, который хранит всё "дерево" наследования
// чтобы работало всё что задумано
struct GPIO : Periph1 {
    // а параметр шаблона определим из конструктора
    GPIO(Address) {}

    // у переферии описываем необходимый метод, передавая в свободную функцию
    // тип переферии, адресс и пробрасываем аргументы
    template<class...Ts>
    static constexpr void set(Ts...args) {
        ::set(Periph1{}, Address::address, args...);
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
    Address(std::size_t address_) { address = address_; }
};

int main() {
    // если необходимо передать адрес динамически (для тестов), тип пока int
    auto address = Address{reinterpret_cast<std::size_t>(new(int))};
    auto mock_gpioa = GPIO{address};
    mock_gpioa.set(Enum1::_1, Enum2::_3, Enum3::_4); // all ok
    // mock_gpioa.set(Enum4::_0);                       // must be compilation error

    // пример без передачи конкретного адреса
    auto gpioa = GPIOA{};
    gpioa.set(Enum1::_1, Enum2::_3, Enum3::_4); // all ok
    // gpioa.set(Enum4::_0);                       // must be compilation error
}