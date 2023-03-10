#pragma once

#include <filesystem>
#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <cmath>
#include <fstream>


namespace omfl {

enum ArgumentType {Int, Float, String, Bool, Array, Space, None};

class ConfigureArgument {
public:
    
    ConfigureArgument(const std::string& name = "", ArgumentType type = None, ConfigureArgument* parent = nullptr, void* const value = nullptr, size_t size = 0) {
            char* to_copy = static_cast<char*>(value);
            valid_ = true;
            value_size_ = size;
            value_type_ = type;
            name_ = name;
            value_ = new char[size];
            parent_ = parent;
            if (value_type_ == Array || value_type_ == Space) {
                
                for (size_t i = 0, id = 0; i < value_size_ / sizeof(ConfigureArgument); i++, id += sizeof(ConfigureArgument)) {
                    ConfigureArgument* copied_arg = reinterpret_cast<ConfigureArgument*>(to_copy + id);
                    ConfigureArgument* arg = new ConfigureArgument(*copied_arg);
                    arg->ChangeParent(this);
                    memcpy(value_ + id, arg, sizeof(ConfigureArgument));
                }
            } else {
                memcpy(value_, to_copy, size);
            }
            
            
    }
    ConfigureArgument(const ConfigureArgument& other)
        : name_(other.GetName())
        , value_type_(other.GetType())
        , value_size_(other.GetSize())
        , parent_(other.GetParent())
        , valid_(other.valid()) {
        value_ = new char[value_size_];
        if (value_type_ == Array || value_type_ == Space) {
            for (size_t i = 0, id = 0; i < value_size_ / sizeof(ConfigureArgument); i++, id += sizeof(ConfigureArgument)) {
                ConfigureArgument* arg = new ConfigureArgument(other[i]);
                arg->ChangeParent(this);
                memcpy(value_ + id, arg, sizeof(ConfigureArgument));
            }
        } else {
            memcpy(value_, other.GetValue(), value_size_);
        }
    }
    ~ConfigureArgument() {
        delete[] value_;
        parent_ = nullptr;
    }
    ConfigureArgument& operator=(const ConfigureArgument& other) {
        
        name_ = other.GetName();
        value_type_ = other.GetType();
        value_size_ = other.GetSize();
        parent_ = other.GetParent();
        valid_ = other.valid();

        delete[] value_;
        value_ = new char[value_size_];
        if (value_type_ == Array || value_type_ == Space) {
            for (size_t i = 0, id = 0; i < value_size_ / sizeof(ConfigureArgument); i++, id += sizeof(ConfigureArgument)) {
                ConfigureArgument* arg = new ConfigureArgument(other[i]);
                arg->ChangeParent(this);
                memcpy(value_ + id, arg, sizeof(ConfigureArgument));
            }
        } else {
            memcpy(value_, other.GetValue(), value_size_);
        }
        return *this;
    }

    void ExprotToXLS(const std::string& filename, const std::string& filepath);

    std::string GetName() const {
        return name_;
    }
    size_t GetSize() const {
        return value_size_;
    }
    ArgumentType GetType() const {
        return value_type_;
    }
    ConfigureArgument* GetParent() const {
        return parent_;
    }
    void ChangeName(const std::string& new_name) {
        name_ = new_name;
    }
    void InValid() {
        valid_ = false;
    }
    bool valid() const;

    

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
        ConfigureArgument* ans;
        if (id < 0 || id >= Length()) {
            ans = new ConfigureArgument;
        } else {
            ans = reinterpret_cast<ConfigureArgument*>(value_ + id * sizeof(ConfigureArgument));
        }
        return *ans;
    }
    void PushBack(const ConfigureArgument& other) {
        ConfigureArgument* to_push = new ConfigureArgument(other);
        char* resized_value_ = new char[value_size_ + sizeof(ConfigureArgument)];
        to_push->ChangeParent(this);
        memcpy(resized_value_, value_, value_size_);
        delete[] value_;
        memcpy(resized_value_ + value_size_, to_push, sizeof(ConfigureArgument));
        value_size_ += sizeof(ConfigureArgument);
        value_ = resized_value_;
    }
    size_t Length() const {
        if (IsArray() || IsSpace()) {
            return value_size_ / sizeof(ConfigureArgument);
        } else {
            return 0;
        }
    }

    // Space
    bool IsSpace() const {
        return value_type_ == Space;
    }
    const ConfigureArgument& Get(const std::string& name) const;
    ConfigureArgument& GetWithCreating(const std::string& name);

    

private:
    std::string name_;
    char* value_;
    size_t value_size_;
    ArgumentType value_type_;
    ConfigureArgument* parent_;
    bool valid_;

    char* GetValue() const {
        return value_;
    }
    void ChangeParent(ConfigureArgument* parent) {
        parent_ = parent;
    }
    
};
// int x = sizeof(ConfigureArgument);

std::ostream& operator << (std::ostream &os, const ConfigureArgument& p);
std::istream& operator >> (std::istream& is, ConfigureArgument& p);

ConfigureArgument& parse(const std::filesystem::path& path);
ConfigureArgument& parse(const std::string& str);
}// namespace
