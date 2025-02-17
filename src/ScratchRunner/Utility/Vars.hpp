#pragma once

#include <variant>
#include <memory>
#include <string>

struct Variable {
    std::string id;
};

struct List {
    std::string id;
};

class ScratchBlock;

using ScratchValue = std::variant<
    double,
    std::string,
    bool,
    Variable,
    List,
    std::shared_ptr<ScratchBlock>
>;

bool isNotExactInteger(double value);
bool valuesAreEqual(ScratchValue val1, ScratchValue val2);