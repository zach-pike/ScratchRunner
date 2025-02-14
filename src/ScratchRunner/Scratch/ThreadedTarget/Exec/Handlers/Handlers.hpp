#pragma once

#include <memory>

#include "ScratchRunner/Scratch/ScratchBlock.hpp"
#include "ScratchRunner/Scratch/ThreadedTarget/ThreadedTarget.hpp"

void motionMoveSteps(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionTurnLeft(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionTurnRight(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionGoToXY(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void motionPointInDirection(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

void controlIf(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
void controlElseIf(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

std::any operatorGt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);
std::any operatorLt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);