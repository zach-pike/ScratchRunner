#pragma once

#include <any>
#include <optional>
#include <string>

// int intFromAny(std::any);
std::optional<float> floatFromAny(const std::any& value);
std::string stringFromAny(const std::any& value);