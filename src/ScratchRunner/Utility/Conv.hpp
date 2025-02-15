#pragma once

#include <any>
#include <optional>
#include <string>

double doubleFromAny(const std::any& value);
std::string stringFromAny(const std::any& value);

bool isValidDouble(const std::string& str);