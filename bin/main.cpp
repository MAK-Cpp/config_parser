#include "lib/parser.h"

using namespace omfl;

void PrintByte(const char& byte) {
    for (uint8_t i = 128; i >= 1; i >>= 1) {
        std::cout << ((byte & i) == i);
    }
}

int main(int, char**) {
    char v[] = "5 * 1e-10";
    float Pi = 3.1415;
    ConfigureArgument 
    str("string argument", v, String, sizeof(v)), 
    fl("float argument", &Pi, Float, sizeof(Pi));
    ConfigureArgument copied = str;
    ConfigureArgument int_mass("int array", nullptr, Array, 0);
    for (int i = 0; i < 5; i++) {
        int chis = (i + 200) * (i + 100) + i + 1234;
        std::string name = "int argument №" + std::to_string(i + 1);
        ConfigureArgument add(name, &chis, Int, sizeof(int));
        int_mass.PushBack(add);
    }
    ConfigureArgument mass[3] = {copied, int_mass, fl};


    ConfigureArgument a("a", mass, Array, sizeof(mass));
   
    for (size_t i = 0; i < a.SizeOfArray(); i++) {
        std::cout << a[i] << '\n';
    }

    return 0;
}
