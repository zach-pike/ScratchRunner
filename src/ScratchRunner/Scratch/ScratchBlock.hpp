#pragma once

#include <any>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <memory>

class ScratchBlock {
public:
    enum class InputType : std::uint8_t {
        Number = 4,
        PosNumber = 5,
        PosInteger = 6,
        Integer = 7,
        Angle = 8,
        Color = 9,
        String = 10,
        Broadcast = 11,
        Variable = 12,
        List = 13,
    };

    std::string opcode;
    std::optional<std::shared_ptr<ScratchBlock>> next;
    std::map<std::string, std::any> inputs;
    std::map<std::string, std::string> fields;
};