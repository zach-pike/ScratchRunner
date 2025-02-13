#include "Conv.hpp"

#include <stdexcept>
#include <exception>

int intFromAny(std::any v) {
    if (v.type() == typeid(int)) {
        return std::any_cast<int>(v);
    } else if (v.type() == typeid(float)) {
        return static_cast<int>(std::any_cast<float>(v));
    }

    throw std::runtime_error("Cant convert type!");
}

int floatFromAny(std::any v) {
    if (v.type() == typeid(int)) {
        return static_cast<float>(std::any_cast<int>(v));
    } else if (v.type() == typeid(float)) {
        return std::any_cast<float>(v);
    }

    throw std::runtime_error("Cant convert type!");
}