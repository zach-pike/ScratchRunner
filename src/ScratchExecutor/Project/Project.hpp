#pragma once

#include "Target/Target.hpp"

#include <vector>

class Project {
private:
    std::vector<Target> targets;
public:
    Project();
    ~Project();
};