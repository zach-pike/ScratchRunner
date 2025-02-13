#include "Exec.hpp"

#include <iostream>

#include "Handlers/Handlers.hpp"

using BlockHandler = std::function<void(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block)>;
static std::map<std::string, BlockHandler> getBlockHandlers() {
    std::map<std::string, BlockHandler> handlers;

    handlers["motion_movesteps"] = motionMoveSteps;
    handlers["motion_turnleft"] = motionTurnLeft;
    handlers["motion_turnright"] = motionTurnRight;
    handlers["motion_gotoxy"] = motionGoToXY;
    handlers["motion_pointindirection"] = motionPointInDirection;

    return handlers;
}

void ExecBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto handlers = getBlockHandlers();

    // First 
    std::optional<std::shared_ptr<ScratchBlock>> current = block;

    while(current.has_value()) {
        auto opcode = current.value()->opcode;

        if (handlers.contains(opcode)) {
            handlers.at(opcode)(target, current.value());
        } else {
            throw std::runtime_error("Invalid block type");
        }

        current = current.value()->next;
    }
}