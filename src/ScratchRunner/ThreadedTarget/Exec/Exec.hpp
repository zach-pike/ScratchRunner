#pragma once

#include "../ThreadedTarget.hpp"
#include "Scratch/ScratchBlock.hpp"
#include <stdexcept>
#include <exception>


void ExecBlock(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block);