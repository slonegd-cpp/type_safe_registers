// abstract concept for 32 bit mcu
#include <cstdint> // uint32_t
#include <type_traits>
#include "type_pack.hpp"



// значение данного типа передаются в функцию, он от него нельзя наследоваться
enum struct Enum1 { _0, _1, _2, _3 };
// поэтому создаётся дополнительный тип, описание свойств,
// в дальнейшем всё равно надо знать как минимум маску
// для определения куда значение записывать
struct Enum1_traits { 
    static constexpr uint32_t mask = 0b111;
};
// нужна функция, связывающая эти 2 типа, используем value based подход
constexpr auto traits(Enum1) { return type_identity<Enum1_traits>{}; }

enum struct Enum2 { _0, _1, _2, _3 };
struct Enum2_traits { 
    static constexpr uint32_t mask = 0b1100;
};
constexpr auto traits(Enum2) { return type_identity<Enum2_traits>{}; }

enum struct Enum3 { _0, _1, _2, _3 };
struct Enum3_traits { 
    static constexpr uint32_t mask = 0b1100;
};
constexpr auto traits(Enum3) { return type_identity<Enum3_traits>{}; }

enum struct Enum4 { _0, _1, _2, _3 };
struct Enum4_traits { 
    static constexpr uint32_t mask = 0b1100;
};
constexpr auto traits(Enum4) { return type_identity<Enum4_traits>{}; }






// наследыванием показываем какие перечисления (их свойства) относятся к какомо регистру
struct Register1 : Enum1_traits, Enum2_traits {
    static constexpr uint32_t address = 0x80000000;
};
struct Register2 : Enum3_traits {
    static constexpr uint32_t address = 0x80000004;
};
struct Register3 : Enum4_traits {
    static constexpr uint32_t address = 0x80000008;
};


// need to change Derived and Base for work with all_of function of type_pack
template <class Derived, class Base>
struct is_base_for {
    static constexpr auto value = std::is_base_of<Base, Derived>::value;
};

template<class Periph, class...Ts>
constexpr void set(Periph periph, Ts...args) {
    // вся магия будет тут
    
    // из аргументов достаём их свойства и упаковываем, используя value based подход
    auto traits_pack = make_type_pack(traits(args)...);
    // и теперь можно проверить все ли свойства аргументов являются базовыми для заданной переферии
    static_assert(all_of(traits_pack, value_fn<is_base_for, Periph>{}));
};


// наследыванием показываем какие регистры относятся к какой переферии
struct Periph1 : Register1 {
    // у переферии описываем необходимый метод, передавая в свободную функцию
    // тип переферии и пробрасываем аргументы
    template<class...Ts>
    static constexpr void set(Ts...args) {
        ::set(Periph1{}, args...);
    }
};
struct Periph2 : Register3 {
    template<class...Ts>
    static constexpr void set(Ts...args) {
        ::set(Periph2{}, args...);
    }
};






int main() {
    auto periph = Periph1{};
    periph.set(Enum1::_1, Enum2::_3); // all ok
    // periph.set(Enum4::_0);            // must be compilation error
}