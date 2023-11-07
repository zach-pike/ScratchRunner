#pragma once

#include <string>

class Meta {
private:
    std::string semver;
    std::string vm;
    std::string agent;

public:
    Meta();
    ~Meta();
};