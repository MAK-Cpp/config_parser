#include "parser.h"


using namespace omfl;

bool ConfigureArgument::valid() const {
    return true;
}   

std::string Parents(const ConfigureArgument* p) {
    
    if (p == nullptr) {
        return "";
    } else if (p->GetParent() == nullptr) {
        return p->GetName();
    }
    return Parents(p->GetParent()) + '.' + p->GetName();
}

ConfigureArgument* ArgumentParser(const std::string& arg, const std::string& name = "Array argument", int id = 0) {
    ConfigureArgument* ans;
    bool is_skobs = false, is_point = false;
    std::string value = "";
    for (; arg[id] == ' ' && id < arg.size(); id++);

    for (; id < arg.size(); id++) {
        if (arg[id] == '.') {
            is_point = true;
        }
        if (arg[id] == '"') {
            is_skobs = true;
            continue;
        }
        value += arg[id];
    }

    if (value == "true" || value == "false") {
        bool v = (value[0] == 't');
        ans = new ConfigureArgument(name, Bool, nullptr, &v, sizeof(v));
    } else if (is_skobs) {
        char v[value.size() + 1];
        value.copy(v, value.size(), 0);
        v[value.size()] = '\0';
        ans = new ConfigureArgument(name, String, nullptr, v, value.size() + 1);
    } else if (is_point) {
        float v = std::stof(value);
        ans = new ConfigureArgument(name, Float, nullptr, &v, sizeof(v));
    } else {
        int v = std::stoi(value);
        ans = new ConfigureArgument(name, Int, nullptr, &v, sizeof(v));
    }
    return ans;
}

std::pair <ConfigureArgument&, int> ArrayParser(const std::string& s, int i = 0) {
    std::pair <ConfigureArgument&, int> ans = {*new ConfigureArgument("Array", Array), -1};
    std::string buff = "";
    for (; i < s.size(); i++) {
        if (s[i] == '[') {
            std::pair <ConfigureArgument&, int> solved_included_array = ArrayParser(s, i + 1);
            i = solved_included_array.second;
            ans.first.PushBack(solved_included_array.first);
            buff = "";
        } else if (s[i] == ']') {
            if (buff != "") {
                ans.first.PushBack(*ArgumentParser(buff));
            } 
            ans.second = i;
            return ans;
        } else {
            if (s[i] == ',') {
                if (buff != "") {
                    ans.first.PushBack(*ArgumentParser(buff));
                    buff = "";
                }
                
            } else {
                buff += s[i];
            }
        }
    }
}

void PrintArray(std::ostream& os, const ConfigureArgument& Arr, int cnt_tabs = 0) {
    for (int i = 0; i < cnt_tabs; i++) {
        os << "\t";
    }
    os << Arr.GetName() << ": [\n";
    for (size_t i = 0; i < Arr.Length(); i++) {
        if (Arr[i].GetType() == Array) {
            PrintArray(os, Arr[i], cnt_tabs + 1);
        } else {
            for (int i = 0; i <= cnt_tabs; i++) {
                os << "\t";
            }
            os << Arr[i] << '\n';
        }
    }
    for (int i = 0; i < cnt_tabs; i++) {
        os << "\t";
    }
    os << "]\n";
}

std::ostream& omfl::operator<<(std::ostream& os, const ConfigureArgument& p) {
    if (p.GetType() == Space) {
        os << '[' << Parents(&p) << ']';
        for (size_t i = 0; i < p.Length(); i++) {
            os << '\n' << p[i] ;
        }
    } else if (p.GetType() == Array) {
        PrintArray(os, p);
    } else {
        os << p.GetName() << ": ";
        if (p.GetType() == Int) {
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
    for (; line[id] != '=' && id < line.size(); id++);
    if (id == line.size()) {
        exit(-1);
    }
    id++;
    for (; line[id] == ' ' && id < line.size(); id++);
    if (line[id] == '[') {
        p = ArrayParser(line, id + 1).first;
        p.ChangeName(name);
    } else {
        p = *ArgumentParser(line, name, id);
    }
    return is;

}

const ConfigureArgument& ConfigureArgument::Get(const std::string& name) const {
    std::vector <std::string> splitted_name;
    std::string buff = "";
    for (auto i: name) {
        if (i == '.') {
            if (buff != "") {
                splitted_name.push_back(buff);
                buff = "";
            }
        } else {
            buff += i;
        }
    }
    if (buff != "") {
        splitted_name.push_back(buff);
    }
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
