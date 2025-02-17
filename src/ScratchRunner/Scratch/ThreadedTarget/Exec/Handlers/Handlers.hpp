#pragma once

#include <memory>

#include "ScratchRunner/Utility/Vars.hpp"
#include "ScratchRunner/Scratch/ScratchBlock.hpp"
#include "ScratchRunner/Scratch/ThreadedTarget/ThreadedTarget.hpp"

void motionMoveSteps(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionTurnLeft(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionTurnRight(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionGoToXY(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionPointInDirection(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

void controlIf(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void controlIfElse(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

ScratchValue operatorAdd(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorSubtract(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorMultiply(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorDivide(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorRandom(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorGt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorLt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorEquals(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorAnd(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorOr(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue operatorNot(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

void dataSetVariableTo(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void dataChangeVariableBy(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

void dataAddToList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void dataDeleteOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void dataDeleteAllOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void dataInsertAtList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void dataReplaceItemOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue dataItemOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue dataItemNumOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue dataLengthOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
ScratchValue dataListContainsItem(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

void debugPrint(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);