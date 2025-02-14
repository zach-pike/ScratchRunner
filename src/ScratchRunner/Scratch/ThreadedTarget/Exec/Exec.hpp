#pragma once

#include "../ThreadedTarget.hpp"
#include "ScratchRunner/Scratch/ScratchBlock.hpp"
#include <stdexcept>
#include <exception>


void ExecBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);

std::any getValueOfReporterBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);