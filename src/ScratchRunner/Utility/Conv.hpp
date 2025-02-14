#pragma once

#include <any>
#include <optional>
#include <string>

std::optional<double> doubleFromAny(const std::any& value);
std::string stringFromAny(const std::any& value);