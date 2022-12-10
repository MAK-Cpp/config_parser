#include "lib/parser.h"

using namespace omfl;

void PrintByte(const char& byte) {
    for (uint8_t i = 128; i >= 1; i >>= 1) {
        std::cout << ((byte & i) == i);
    }
}

int main(int, char**) {
    ConfigureArgument root(".Base.", Space);
    int n, m;
    std::cin >> n;
    ConfigureArgument buff;
    for (int i = 0; i < n; i++) {
        std::cin >> buff;
        // std::cout << buff << '\n';
        root.PushBack(buff);
    }
    std::cout << root << '\n';
    ConfigureArgument numbers("My lovely numbers", Space);
    /*
    добавить к классу переменную префикса, чтобы для пространств
    выводить все имя, хотя это и не обязательно, но для приличия

    ИЛИ

    добавить указатель на родительский класс

    +

    написать ф-цию, которая на вход получает строку, описывающую массив аргументов и возвращающая пару типа
        Аргумент-Массив : индекс, где остановился
    */

    return 0;
}
