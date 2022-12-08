#pragma once

#include <filesystem>
#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <tuple>

void PrintByte(const char& byte) {
    for (uint8_t i = 128; i >= 1; i >>= 1) {
        std::cout << ((byte & i) == i);
    }
}

namespace omfl {

enum ArgumentType {Int, Float, String, Bool, Array, None};

class ConfigureArgument {
public:
    ConfigureArgument(const std::string& name = "", void* const value = nullptr, ArgumentType type = None, size_t size = 0) 
        : name_(name) {
            char* to_copy = static_cast<char*>(value);
            value_ = new char(size);
            value_type_ = type;
            value_size_ = size;
            memcpy(value_, to_copy, size);
            // for (size_t i = size - 1; i >= 0; i--) {
            //     PrintByte(value_[i]);
            //     std::cout << ' ';
            //     if (i == 0) break;
            // }
            // std::cout << '\n';
    }

    // integer
    bool IsInt() const {
        return value_type_ == Int;
    }
    int AsIntOrDefault(int my_default) const {
        if (!IsInt()) {
            return my_default;
        }
        return AsInt();
    }
    int AsInt() const {
        int ans = value_[value_size_ - 1];
        for (size_t i = value_size_ - 2; i >= 0; i--) {
            ans = (ans << 8) + static_cast<uint8_t>(value_[i]);
            if (i == 0) break;
        }
        return ans;
    }

    // float
    bool IsFloat() const {
        return value_type_ == Float;
    }
    float AsFloatOrDefault(float my_default) const {
        if (!IsFloat()) {
            return my_default;
        }
        return AsFloat();
    }
    float AsFloat() const {
        bool is_negative = ((value_[value_size_ - 1] & 128) == 128);
        float exp = ((value_[value_size_ - 1] & 0b01111111) << 1) + ((value_[value_size_ - 2] & 0b10000000) == 0b10000000) - 127;
        float delta = std::pow(2, exp);
        float ans = delta;
        delta /= 2;
        for (size_t i = value_size_ - 2; i >= 0; i--) {
            int bit_id = 128;
            if (i == value_size_ - 2) bit_id >>= 1;
            while (bit_id > 0) {
                if((value_[i] & bit_id) == bit_id) {
                    ans += delta;
                }
                bit_id >>= 1;
                delta /= 2;
            }
            if (i == 0) break;
        }
        return (is_negative ? -ans : ans);
    }

    // string
    bool IsString() const{
        return value_type_ == String;
    }
    
    // bool
    bool IsBool() const {
        return value_type_ == Bool;
    }

    // array
    bool IsArray() const {
        return value_type_ == Array;
    }
    
    ~ConfigureArgument() {
        delete value_;
    }
private:
    std::string name_;
    char* value_;
    size_t value_size_;
    ArgumentType value_type_;
};

class ConfigureSpace {
public:
    ConfigureSpace(const std::string& name = ".EmptySpase."): argumets_(0) {
        name_ = name;
        nested_spaces_ = {};
        // argumets_ = {};
        // argumets_ = *new ConfigureMassive(".EmtpyMassive.");
    }
    ConfigureArgument& Get(const std::string&) const;

    
    bool valid() const;
private:
    std::string name_;
    std::vector <ConfigureSpace> nested_spaces_;
    std::vector <ConfigureArgument> argumets_;
};

ConfigureSpace parse(const std::filesystem::path& path);
ConfigureSpace parse(const std::string& str);

}// namespace