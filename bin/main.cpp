#include "lib/parser.h"

using namespace omfl;

void PrintByte(const char& byte) {
    for (uint8_t i = 128; i >= 1; i >>= 1) {
        std::cout << ((byte & i) == i);
    }
}

int main(int, char**) {
    ConfigureArgument root("__Base__", Space);
    int n, m;
    std::cin >> n;
    ConfigureArgument buff;
    for (int i = 0; i < n; i++) {
        std::cin >> buff;
        std::cout << buff << '\n';
        root.PushBack(buff);
    }
    std::cout << root << '\n';
    ConfigureArgument numbers("My lovely numbers", Space);
    std::cin >> m;
    for (int i = 0; i < m; i++) {
        std::cin >> buff;
        numbers.PushBack(buff);
    }
    root.PushBack(numbers);
    std::cout << root << '\n';
    /*
    добавить к классу переменную префикса, чтобы для пространств
    выводить все имя, хотя это и не обязательно, но для приличия

    ИЛИ

    добавить указатель на родительский класс

    +

    написать ф-цию, которая на вход получает строку, описывающую массив аргументов и возвращающая пару типа
        Аргумент-Массив : индекс, где остановился

1
big_hard_massive = [["hello! how are you?"], 0.000001, [[2, 3], [true, false]], 5] 
5
key1 = 10000
key2 = -3333
key3 = -3.1415
key4 = "Hello Wolrd!"
key5 = false


6  
1 2
3 4
5 6
7 8
9 10
11 12

key1  [1, true, 3.14, "ITMO", [1, 2, 3], ["a", "b", 28]]
    */




    return 0;
}
