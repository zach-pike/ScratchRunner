#pragma once

#include "ScratchBlock.hpp"
#include "ScratchCostume.hpp"

#include <map>

class ScratchTarget {
public:
    bool isStage;
    std::string name;
    std::map<std::string, std::any> variables;
    std::map<std::string, std::vector<std::any>> lists;
    
    std::vector<std::shared_ptr<ScratchCostume>> costumes;

    // lists WIP
    // broadcasts WIP
    std::vector<std::shared_ptr<ScratchBlock>> blocks;

    int currentCostume;
    float posX;
    float posY;
    float size;
    float direction;
    bool draggable;
    bool visible;

    int layerOrder;
};