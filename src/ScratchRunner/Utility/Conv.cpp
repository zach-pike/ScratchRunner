#include "Conv.hpp"

#include <stdexcept>
#include <exception>
#include <iostream>

std::optional<double> doubleFromAny(const std::any& value) {
    try {
        if (value.type() == typeid(double)) {
            return std::any_cast<double>(value);
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
    if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    }
    return "";
}