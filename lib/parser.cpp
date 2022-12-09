#include "parser.h"

using namespace omfl;

std::ostream& omfl::operator<<(std::ostream& os, const ConfigureArgument& p) {
    os << p.GetName() << ' ';
    if (p.GetType() == Array) {
        os << '\n';
        for (size_t i = 0; i < p.SizeOfArray(); i++) {
            os << '\t' << p[i];
            if (i != p.SizeOfArray() - 1) os << '\n';
        }
    } else if (p.GetType() == Int) {
        os << p.AsInt();
    } else if (p.GetType() == Float) {
        os << p.AsFloat();
    } else if (p.GetType() == String) {
        os << p.AsString();
    } else if (p.GetType() == Bool) {
        os << p.AsBool();
    } else {
        os << "ERROR";
    }
    return os;
}

// omfl::ConfigureArgument& omfl::ConfigureSpace::Get(const std::string&) const {

// }