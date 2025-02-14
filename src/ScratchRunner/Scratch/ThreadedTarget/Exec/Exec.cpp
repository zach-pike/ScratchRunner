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

    handlers["control_if"] = controlIf;
    handlers["control_if_else"] = controlElseIf;

    return handlers;
}

using ReporterHandler = std::function<std::any(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block)>;
static std::map<std::string, ReporterHandler> getReporterHandlers() {
    std::map<std::string, ReporterHandler> handlers;

    handlers["operator_lt"] = operatorLt;
    handlers["operator_gt"] = operatorGt;

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

std::any getValueOfReporterBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto handlers = getReporterHandlers();

    assert(handlers.contains(block->opcode));

    return handlers.at(block->opcode)(target, block);
}