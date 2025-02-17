#include "Vars.hpp"

#include <cmath>
#include <string>

#include "Conv.hpp"

bool isNotExactInteger(double value) {
    return std::floor(value) != value;
}

bool valuesAreEqual(std::any val1, std::any val2) {
    if (val1.type() == typeid(std::string) || val2.type() == typeid(std::string)) {
        std::string sparam1 = stringFromAny(val1);
        std::string sparam2 = stringFromAny(val2);

        return sparam1 == sparam2;
    } else {
        // Treat as numbers
        double dparam1 = doubleFromAny(val1);
        double dparam2 = doubleFromAny(val2);

        return dparam1 == dparam2;
    }
}