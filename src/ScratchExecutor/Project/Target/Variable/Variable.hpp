#pragma once

#include <string>
#include <any>

class Variable {
private:
    std::string realVarName;
    std::any value;
public:
    Variable();
    ~Variable();
};