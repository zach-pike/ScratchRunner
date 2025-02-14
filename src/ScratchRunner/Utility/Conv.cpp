#include "Conv.hpp"

#include <stdexcept>
#include <exception>
#include <iostream>

// int intFromAny(std::any v) {
//     if (v.type() == typeid(int)) {
//         return std::any_cast<int>(v);
//     } else if (v.type() == typeid(float)) {
//         return static_cast<int>(std::any_cast<float>(v));
//     } else if (v.type() == typeid(std::string)) {
//         return std::stof(std::any_cast<std::string>(v));
//     }

//     throw std::runtime_error("Cant convert type!");
// }

std::optional<float> floatFromAny(const std::any& value) {
    try {
        if (value.type() == typeid(float)) {
            return std::any_cast<float>(value);
        }
        if (value.type() == typeid(int)) {
            return static_cast<float>(std::any_cast<int>(value));
        }
        if (value.type() == typeid(std::string)) {
            std::string strValue = std::any_cast<std::string>(value);
            if (!strValue.empty() && (std::isdigit(strValue[0]) || strValue[0] == '-' || strValue[0] == '.')) {
                return std::stof(strValue);
            }
        }
    } catch (...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::string stringFromAny(const std::any& value) {
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    }
    if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    }
    if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    }
    return "";
}