#include "Vars.hpp"

#include <cmath>
#include <string>

#include "Conv.hpp"

bool isNotExactInteger(double value) {
    return std::floor(value) != value;
}

bool valuesAreEqual(ScratchValue val1, ScratchValue val2) {
    if (std::holds_alternative<std::string>(val1) || std::holds_alternative<std::string>(val2)) {
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