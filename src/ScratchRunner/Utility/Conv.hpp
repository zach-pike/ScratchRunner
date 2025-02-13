#pragma once

#include <any>

#include "ScratchRunner/Scratch/ThreadedTarget/ThreadedTarget.hpp"

int intFromAny(ThreadedTarget* target, std::any);
int floatFromAny(ThreadedTarget* target, std::any);