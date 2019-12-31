// abstract concept for 32 bit mcu
#include <cstdint> // uint32_t


struct Enum1 {
    static constexpr uint32_t mask = 0b111;
    enum {_0, _1, _2, _3};
};
struct Enum2 {
    static constexpr uint32_t mask = 0b1100;
    enum {_0, _1, _2};
};
struct Enum3 {
    static constexpr uint32_t mask = 0b1100;
    enum {_0, _1, _2};
};
struct Enum4 {
    static constexpr uint32_t mask = 0b1100;
    enum {_0, _1, _2};
};


// bind types by inheritance
struct Register1 : Enum1, Enum2 {
    static constexpr uint32_t address = 0x80000000;
};
struct Register2 : Enum3 {
    static constexpr uint32_t address = 0x80000004;
};
struct Register3 : Enum4 {
    static constexpr uint32_t address = 0x80000008;
};


template<class Periph, class...Ts>
void set(Periph periph, Ts...args) {
    // all magic here
};


struct Periph1 : Register1, Register2 {
    template<class...Ts>
    void set(Ts...args) {
        ::set(Periph1{}, args...);
    }
};
struct Periph2 : Register3 {
    template<class...Ts>
    void set(Ts...args) {
        ::set(Periph2{}, args...);
    }
};




int main() {
    auto periph = Periph1{};
    periph.set(Enum1::_1, Enum3::_2); // all ok
    periph.set(Enum4::_0);            // must be compilation error
}