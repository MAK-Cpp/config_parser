#include "parser.h"

using namespace omfl;

bool ConfigureArgument::valid() const {
    return true;
}   

std::ostream& omfl::operator<<(std::ostream& os, const ConfigureArgument& p) {
    if (p.GetType() == Space) {
        os << '[' << p.GetName() << "]\n";
        for (size_t i = 0; i < p.Length(); i++) {
            os << p[i] << '\n';
        }
    } else {
        os << p.GetName() << ": ";
    if (p.GetType() == Array) {
        os << "{\n";
        for (size_t i = 0; i < p.Length(); i++) {
            os << '\t' << p[i] << '\n';
        }
        os << "}";
    } else if (p.GetType() == Int) {
        os << p.AsInt();
    } else if (p.GetType() == Float) {
        os << p.AsFloat();
    } else if (p.GetType() == String) {
        os << '"' << p.AsString() << '"';
    } else if (p.GetType() == Bool) {
        os << p.AsBool();
    } else {
        os << "ERROR";
    }
    }

    
    return os;
}

std::istream& omfl::operator>>(std::istream& is, ConfigureArgument& p) {
    std::string line, name = "", value = "";
    bool is_skobs = false, is_point = false;
    is >> name;
    char sp;
    std::getline(is, line);
    
    int id = 0;
    for (; line[id] == ' ' && id < line.size(); id++);

    for (; id < line.size(); id++) {
        if (line[id] == '.') {
            is_point = true;
        }
        if (line[id] == '"') {
            is_skobs = true;
            continue;
        }
        value += line[id];
    }

    if (value == "true" || value == "false") {
        bool v = (value[0] == 't');
        p = *new ConfigureArgument(name, Bool, &v, sizeof(v));
    } else if (is_skobs) {
        char v[value.size() + 1];
        value.copy(v, value.size(), 0);
        v[value.size()] = '\0';
        p = *new ConfigureArgument(name, String, v, value.size() + 1);
    } else if (is_point) {
        float v = std::stof(value);
        p = *new ConfigureArgument(name, Float, &v, sizeof(v));
    } else {
        int v = std::stoi(value);
        p = *new ConfigureArgument(name, Int, &v, sizeof(v));
    }
    return is;

}

const ConfigureArgument& ConfigureArgument::Get(const std::string& name) const {
    std::vector <std::string> splitted_name;
    std::string buff = "";
    for (auto i: name) {
        if (i == '.' && buff != "") {
            splitted_name.push_back(buff);
            buff = "";
        } else {
            buff += i;
        }
    }
    if (buff != "") {
        splitted_name.push_back(buff);
    }
    // for (auto i: splitted_name) std::cout << i << ' ';
    // std::cout << '\n';
    // for (size_t i = 0; i < Length(); i++) {
    //     std::cout << (*this)[i] << ' ';
    // }
    // std::cout << '\n';
    int id = 0;
    const ConfigureArgument* ans = this;
    for (size_t i = 0; id < splitted_name.size(); i++) {
        if (i == ans->Length()) {
            break;
        }
        if ((*ans)[i].GetName() == splitted_name[id]) {
            ans = &((*ans)[i]);
            id++;
            i = 0;
        }
        if (id == splitted_name.size()) {
            break;
        }
    }
    return *ans;
}