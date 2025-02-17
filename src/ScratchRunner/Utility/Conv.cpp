#include "Conv.hpp"

#include <stdexcept>
#include <exception>
#include <iostream>
#include <charconv>

double doubleFromAny(const ScratchValue& value) {
    try {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        if (std::holds_alternative<std::string>(value)) {
            std::string strValue = std::get<std::string>(value);
            if (!strValue.empty() && (std::isdigit(strValue[0]) || strValue[0] == '-' || strValue[0] == '.')) {
                return std::stof(strValue);
            }
        }
    } catch (...) {
        return 0;
    }
    return 0;
}

std::string stringFromAny(const ScratchValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
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