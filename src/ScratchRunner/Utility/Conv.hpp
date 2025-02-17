#pragma once

#include <optional>
#include <string>

#include "ScratchRunner/Utility/Vars.hpp"

double doubleFromAny(const ScratchValue& value);
std::string stringFromAny(const ScratchValue& value);

bool isValidDouble(const std::string& str);