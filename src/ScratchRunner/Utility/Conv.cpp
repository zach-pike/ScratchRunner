#include "Conv.hpp"

#include <stdexcept>
#include <exception>
#include <iostream>
#include <charconv>

double doubleFromAny(const std::any& value) {
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
        return 0;
    }
    return 0;
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

bool isValidDouble(const std::string& str) {
    if (str.empty()) return false;  // Handle empty string case

    const char* begin = str.data();
    const char* end = begin + str.size();
    double value; 

    auto [ptr, ec] = std::from_chars(begin, end, value, std::chars_format::general);

    return ec == std::errc{} && ptr == end;  // Ensure full parsing
}