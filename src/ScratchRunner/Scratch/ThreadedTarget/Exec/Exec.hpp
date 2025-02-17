#pragma once

#include "../ThreadedTarget.hpp"
#include "ScratchRunner/Scratch/ScratchBlock.hpp"
#include <stdexcept>
#include <exception>

#include "ScratchRunner/Utility/Vars.hpp"

void ExecBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

ScratchValue getValueOfReporterBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);