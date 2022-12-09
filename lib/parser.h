#pragma once

#include <filesystem>
#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <tuple>



namespace omfl {

enum ArgumentType {Int, Float, String, Bool, Array, None};

#pragma pack(1)
class ConfigureArgument {
public:
    ConfigureArgument(const std::string& name = "", void* const value = nullptr, ArgumentType type = None, size_t size = 0) {
            char* to_copy = static_cast<char*>(value);
            value_size_ = size;
            value_type_ = type;
            name_ = name;
            value_ = new char[size];
            if (value_type_ == Array) {
                
                for (size_t i = 0, id = 0; i < size / sizeof(ConfigureArgument); i++, id += sizeof(ConfigureArgument)) {
                    ConfigureArgument* copied_arg = reinterpret_cast<ConfigureArgument*>(to_copy + id);
                    ConfigureArgument* arg = new ConfigureArgument(*copied_arg);
                    memcpy(value_ + id, arg, sizeof(ConfigureArgument));
                    arg = reinterpret_cast<ConfigureArgument*>(value_ + id);

                    // std::cout << id << ' ' << arg->IsFloat() << '\n';
                }
            } else {
                memcpy(value_, to_copy, size);
            }
            
            
    }
    ConfigureArgument(const ConfigureArgument& other)
        : name_(other.GetName())
        , value_type_(other.GetType())
        , value_size_(other.GetSize()) {
        value_ = new char[value_size_];
        if (value_type_ == Array) {
            for (size_t i = 0, id = 0; i < value_size_ / sizeof(ConfigureArgument); i++, id += sizeof(ConfigureArgument)) {
                ConfigureArgument* arg = new ConfigureArgument(other[i]);
                memcpy(value_ + id, arg, sizeof(ConfigureArgument));
                arg = reinterpret_cast<ConfigureArgument*>(value_ + id);
            }
        } else {
            memcpy(value_, other.GetValue(), value_size_);
        }
    }
    ConfigureArgument& operator=(const ConfigureArgument& other) {
        
        name_ = other.GetName();
        value_type_ = other.GetType();
        value_size_ = other.GetSize();

        delete[] value_;
        value_ = new char[value_size_];
        if (value_type_ == Array) {
            for (size_t i = 0, id = 0; i < value_size_ / sizeof(ConfigureArgument); i++, id += sizeof(ConfigureArgument)) {
                ConfigureArgument* arg = new ConfigureArgument(other[i]);
                memcpy(value_ + id, arg, sizeof(ConfigureArgument));
                arg = reinterpret_cast<ConfigureArgument*>(value_ + id);

            }
        } else {
            memcpy(value_, other.GetValue(), value_size_);
        }
        return *this;
    }

    std::string GetName() const {
        return name_;
    }
    size_t GetSize() const {
        return value_size_;
    }
    ArgumentType GetType() const {
        return value_type_;
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
        int ans = *reinterpret_cast<int*>(value_);
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
        float ans = *reinterpret_cast<float*>(value_);
        return ans;
    }

    // string
    bool IsString() const{
        return value_type_ == String;
    }
    std::string AsStringOrDefault(const std::string& my_default) const {
        if (value_type_ != String) {
            return my_default;
        }
        return AsString();
    }
    std::string AsString() const {
        return value_;
    }
    
    // bool
    bool IsBool() const {
        return value_type_ == Bool;
    }
    bool AsBoolOrDefault(bool my_default) const {
        if (!IsBool()) {
            return my_default;
        }
        return AsBool();
    }
    bool AsBool() const {
        bool ans = *reinterpret_cast<bool*>(value_);
        return ans;
    }

    // array
    bool IsArray() const {
        return value_type_ == Array;
    }
    ConfigureArgument& operator[](int id) const {
        ConfigureArgument* ans = reinterpret_cast<ConfigureArgument*>(value_ + id * sizeof(ConfigureArgument));
        return *ans;
    }
    void PushBack(const ConfigureArgument& other) {
        ConfigureArgument* to_push = new ConfigureArgument(other);
        char* resized_value_ = new char[value_size_ + sizeof(ConfigureArgument)];
        memcpy(resized_value_, value_, value_size_);
        delete[] value_;
        memcpy(resized_value_ + value_size_, to_push, sizeof(ConfigureArgument));
        value_size_ += sizeof(ConfigureArgument);
        value_ = resized_value_;
        
    }
    size_t SizeOfArray() const {
        if (IsArray()) {
            return value_size_ / sizeof(ConfigureArgument);
        } else {
            return 0;
        }
    }
    
    ~ConfigureArgument() {
        // std::cout << "Deleting " << name_ << " with adress " << this << "... ";

        delete[] value_;
        
        
        // std::cout << "Done!\n";
    }
private:
    std::string name_;
    char* value_;
    size_t value_size_;
    ArgumentType value_type_;

    char* GetValue() const {
        return value_;
    }
    
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

std::ostream& operator << (std::ostream &os, const ConfigureArgument &p);

ConfigureSpace parse(const std::filesystem::path& path);
ConfigureSpace parse(const std::string& str);
#pragma pop()
}// namespace

