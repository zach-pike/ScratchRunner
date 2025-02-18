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
    handlers["control_if_else"] = controlIfElse;

    handlers["data_setvariableto"] = dataSetVariableTo;
    handlers["data_changevariableby"] = dataChangeVariableBy;

    handlers["data_addtolist"] = dataAddToList;
    handlers["data_deleteoflist"] = dataDeleteOfList;
    handlers["data_deletealloflist"] = dataDeleteAllOfList;
    handlers["data_insertatlist"] = dataInsertAtList;
    handlers["data_replaceitemoflist"] = dataReplaceItemOfList;

    // Used for debugging
    // handlers["looks_sayforsecs"] = debugPrint;

    return handlers;
}

using ReporterHandler = std::function<ScratchValue(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block)>;
static std::map<std::string, ReporterHandler> getReporterHandlers() {
    std::map<std::string, ReporterHandler> handlers;

    handlers["operator_add"] = operatorAdd;
    handlers["operator_subtract"] = operatorSubtract;
    handlers["operator_multiply"] = operatorMultiply;
    handlers["operator_divide"] = operatorDivide;
    handlers["operator_random"] = operatorRandom;
    handlers["operator_lt"] = operatorLt;
    handlers["operator_gt"] = operatorGt;
    handlers["operator_equals"] = operatorEquals;
    handlers["operator_and"] = operatorAnd;
    handlers["operator_or"] = operatorOr;
    handlers["operator_not"] = operatorNot;

    handlers["data_itemoflist"] = dataItemOfList;
    handlers["data_itemnumoflist"] = dataItemNumOfList;
    handlers["data_lengthoflist"] = dataLengthOfList;
    handlers["data_listcontainsitem"] = dataListContainsItem;

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

ScratchValue getValueOfReporterBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto handlers = getReporterHandlers();

    // assert(handlers.contains(block->opcode));
    if (!handlers.contains(block->opcode)) {
        std::cout << block->opcode  << '\n';
        assert(false);
    }

    return handlers.at(block->opcode)(target, block);
}