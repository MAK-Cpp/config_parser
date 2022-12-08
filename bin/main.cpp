#include "lib/parser.h"

using namespace omfl;

int main(int, char**) {
    float w = std::sqrt(2);

    ConfigureArgument a("a", &w, Float, sizeof(float));
    std::cout << a.AsFloat();
    return 0;
}
