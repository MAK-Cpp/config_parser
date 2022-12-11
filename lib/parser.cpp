#include "parser.h"


using namespace omfl;

bool ConfigureArgument::valid() const {
    return valid_;
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
    int is_skobs = 0;
    int is_point = 0;
    bool not_numbers = false;
    std::string value = "";
    for (; arg[id] == ' ' && id < arg.size(); id++);

    for (; id < arg.size(); id++) {
        if (!('0' <= arg[id] && arg[id] <= '9' || 
                                arg[id] == '.' || 
                                arg[id] == '+' || 
                                arg[id] == '-' || 
                                arg[id] == ' ')) {
            not_numbers = true;
        }
        if (arg[id] == '+' || arg[id] == '-') {
            if (is_skobs == 0 && value != "") {
                ans = new ConfigureArgument;
                ans->InValid();
                return ans;
            }
        } if (arg[id] == '.') {
            if (is_skobs > 0) {
                value += arg[id];
                continue;
            }
            if (value == "" || id == arg.size() - 1) {
                ans = new ConfigureArgument;
                ans->InValid();
                return ans;
            } else if (!isalnum(arg[id - 1]) || !isalnum(arg[id + 1])) {
                ans = new ConfigureArgument;
                ans->InValid();
                return ans;
            }
            is_point++;
            value += arg[id];
            if (is_point > 1) {
                ans = new ConfigureArgument;
                ans->InValid();
                return ans;
            }
        } else if (arg[id] == '"') {
            is_skobs++;
            continue;
        } else {
            value += arg[id];
        }
        
    }

    if (value == "true" || value == "false") {
        bool v = (value[0] == 't');
        ans = new ConfigureArgument(name, Bool, nullptr, &v, sizeof(v));
    } else if (is_skobs == 2) {
        char v[value.size() + 1];
        value.copy(v, value.size(), 0);
        v[value.size()] = '\0';
        ans = new ConfigureArgument(name, String, nullptr, v, value.size() + 1);
    } else if (is_point == 1) {
        float v;
        try {
            if (not_numbers) {
                throw -1;
            }
            v = std::stof(value);   
        }
        catch(...) {
            ans = new ConfigureArgument;
            ans->InValid();
            return ans;
        }
        ans = new ConfigureArgument(name, Float, nullptr, &v, sizeof(v));
    } else {
        int v;
        try {
            if (not_numbers) {
                throw -1;
            }
            v = std::stoi(value);
        } 
        catch (...) {
            ans = new ConfigureArgument;
            ans->InValid();
            return ans;
        }
        ans = new ConfigureArgument(name, Int, nullptr, &v, sizeof(v));
    }
    return ans;
}

std::pair <ConfigureArgument*, int> ArrayParser(const std::string& s, int i = 0) {
    std::pair <ConfigureArgument*, int> ans = {new ConfigureArgument("Array", Array), -1};
    std::string buff = "";
    for (; i < s.size(); i++) {
        if (s[i] == '[') {
            std::pair <ConfigureArgument*, int> solved_included_array = ArrayParser(s, i + 1);
            if (!solved_included_array.first->valid()) {
                ans.first->InValid();
                return ans;
            }
            i = solved_included_array.second;
            ans.first->PushBack(*solved_included_array.first);
            buff = "";
        } else if (s[i] == ']') {
            if (buff != "") {
                ConfigureArgument* to_push = ArgumentParser(buff);
                if (!to_push->valid()) {
                    ans.first->InValid();
                    return ans;
                } 
                ans.first->PushBack(*to_push);
            } 
            ans.second = i;
            // if (ans.first->Length() == 0) {
            //     ans.first->InValid();
            // }
            return ans;
        } else {
            if (s[i] == ',') {
                if (buff != "") {
                    ConfigureArgument* to_push = ArgumentParser(buff);
                    if (!to_push->valid()) {
                        ans.first->InValid();
                        return ans;
                    }
                    ans.first->PushBack(*to_push);
                    buff = "";
                }
                
            } else {
                buff += s[i];
            }
        }
    }
    ans.first->InValid();
    return ans;
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
        p = *(ArrayParser(line, id + 1).first);
        p.ChangeName(name);
    } else {
        p = *ArgumentParser(line, name, id);
    }
    return is;

}

const ConfigureArgument& ConfigureArgument::Get(const std::string& name) const {
    std::vector <std::string> splitted_name;
    ConfigureArgument* invalid_argument = new ConfigureArgument;
    invalid_argument->InValid();
    std::string buff = "";
    for (auto i: name) {
        if (i == '.') {
            if (buff != "") {
                splitted_name.push_back(buff);
                buff = "";
            } else {
                return *invalid_argument;
            }
        } else {
            buff += i;
        }
    }
    if (buff != "") {
        splitted_name.push_back(buff);
    } else {
        return *invalid_argument;
    }
    int id = 0;
    const ConfigureArgument* ans = this;
    for (int i = 0; id < splitted_name.size(); i++) {
        if (i == ans->Length()) {
            return *invalid_argument;
        } else if ((*ans)[i].GetName() == splitted_name[id]) {
            ans = &((*ans)[i]);
            id++;
            i = -1;
        }
    }
    return *ans;
}


ConfigureArgument& ConfigureArgument::GetWithCreating(const std::string& name) {
    std::vector <std::string> splitted_name;
    std::string buff = "";
    for (auto i: name) {
        if (i == '.') {
            if (buff != "") {
                splitted_name.push_back(buff);
                buff = "";
            } else {
                InValid();
                return *this;
            }
        } else {
            buff += i;
        }
    }
    if (buff != "") {
        splitted_name.push_back(buff);
    } else {
        InValid();
        return *this;
    }
    int id = 0;
    ConfigureArgument* ans = this;
    for (int i = 0; id < splitted_name.size(); i++) {
        if (i == ans->Length()) {
            ConfigureArgument* new_space = new ConfigureArgument(splitted_name[id], Space);
            ans->PushBack(*new_space);
            ans = &((*ans)[ans->Length() - 1]);
            id++;
            i = -1;
        } else if ((*ans)[i].GetName() == splitted_name[id]) {
            ans = &((*ans)[i]);
            id++;
            i = -1;
        }
    }
    return *ans;
}

ConfigureArgument& omfl::parse(const std::string& str) {
    ConfigureArgument* root = new ConfigureArgument("__ROOT__", Space);
    ConfigureArgument* workplace = root;
    std::string argument = "", value = "";
    bool equal_sign = false;
    bool end_argument_name = false;
    bool is_section = false;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '#') {
            while (i < str.size() && str[i] != '\n' && str[i] != '\0') i++;
        }
        if (str[i] == '\n' || str[i] == '\0') {
            if ((argument != "" && value == "") || (argument == "" && value != "")) {
                root->InValid();
                return *root;
            } else if (argument != "" && value != "") {
                if (workplace->Get(argument).valid()) {
                    root->InValid();
                    return *root;
                }
                ConfigureArgument* add;
                if (value[0] == '[') {
                    add = ArrayParser(value, 1).first;
                    add->ChangeName(argument);
                } else {
                    add = ArgumentParser(value, argument);
                }
                if (add->valid()) {
                    workplace->PushBack(*add);
                } else {
                    root->InValid();
                    return *root;
                }
            }
            argument = "";
            value = "";
            equal_sign = false;
            end_argument_name = false;
            is_section = false;
            continue;
        } 
        if (is_section) {
            if (str[i] == ']') {
                workplace = &root->GetWithCreating(argument);
                if (!workplace->valid()) {
                    return *root;
                }
                while (i < str.size() && str[i] != '\n' && str[i] != '\0') i++;
                argument = "";
                value = "";
                equal_sign = false;
                end_argument_name = false;
                is_section = false;
                continue;
            } else {
                argument += str[i];
            }
        } else {
            if (equal_sign) {
                if (str[i] == ' ') {
                    if (value != "") {
                        value += str[i];
                    }
                } else {
                    value += str[i];
                }
            } else {
                if (str[i] == ' ') {
                    if (argument != "") {
                        end_argument_name = true;
                    }
                } else if (str[i] == '=') {
                    equal_sign = true;
                } else {
                    if (str[i] == '[' && argument == "") {
                        is_section = true;
                        continue;
                    }
                    if (!std::isalnum(str[i]) && str[i] != '-' && str[i] != '_') {
                        root->InValid();
                        return *root;
                    } else if (end_argument_name) {
                        root->InValid();
                        return *root;
                    } else {
                        argument += str[i];
                    }
                }
            }
        }
        
    }
    if ((argument != "" && value == "") || (argument == "" && value != "")) {
        root->InValid();
    } else if (argument != "" && value != "") {
        if (workplace->Get(argument).valid()) {
            root->InValid();
            return *root;
        } 
        ConfigureArgument* add;
        if (value[0] == '[') {
            add = ArrayParser(value, 1).first;
            add->ChangeName(argument);
        } else {
            add = ArgumentParser(value, argument);
        }
        if (add->valid()) {
            workplace->PushBack(*add);
        } else {
            root->InValid();
        }
    }
    return *root;
}

ConfigureArgument& omfl::parse(const std::filesystem::path& path) {
    std::string to_parse = "";
    char x;
    std::ifstream file;
    file.open(path);
    while (file.get(x)) {
        to_parse += x;
    }
    return parse(to_parse);
}