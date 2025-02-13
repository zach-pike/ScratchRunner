#pragma once

#include "ScratchTarget.hpp"

#include <vector>
#include <memory>

class ScratchProject {
public:
    std::vector<std::shared_ptr<ScratchTarget>> targets;
};