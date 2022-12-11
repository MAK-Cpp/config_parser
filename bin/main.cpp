#include "lib/parser.h"

using namespace omfl;

int main(int, char**) {
    std::filesystem::path file = "C:/Users/User/Desktop/c++/ITMO/BaseOfProgramming/labwork-6-N4R1CK/example/config.omfl";
    
    ConfigureArgument x = parse(file);
    std::cout << x;
    return 0;
}
