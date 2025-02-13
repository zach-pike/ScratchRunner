#pragma once

#include "ScratchProject.hpp"

#include <memory>

std::shared_ptr<ScratchProject> ParseProjectJSON(std::string filepath);