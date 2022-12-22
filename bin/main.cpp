#include "lib/parser.h"
#include <sys/stat.h>


using namespace omfl;

int main(int argc, char** argv) {
    std::filesystem::path current_file = argv[0];
    std::filesystem::path file = current_file.parent_path().string() + "./../example/config.omfl";
    
    ConfigureArgument x = parse(file);
    std::cout << x;
    x.ExprotToXLS("CheckResuts", current_file.parent_path().string());
    
    return 0;
}