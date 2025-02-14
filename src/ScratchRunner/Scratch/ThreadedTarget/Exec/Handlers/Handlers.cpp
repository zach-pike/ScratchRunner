#include "Handlers.hpp"

#include "ScratchRunner/Utility/Conv.hpp"
#include "../Exec.hpp"
#include "ScratchRunner/Runner.hpp"

#include <iostream>


#define DEG2RAD (M_PI / 180.f)

static std::any resolveValue(ThreadedTarget* target, std::any v) {
    if (v.type() == typeid(int) ||
        v.type() == typeid(float) ||
        v.type() == typeid(std::string)
    ) {
        return v;
    } else if (v.type() == typeid(Variable)) {
        Variable var = std::any_cast<Variable>(v);
        auto val = target->getVariable(var.id);

        assert(val.has_value());

        return resolveValue(target, val.value());
    } else {
        throw std::runtime_error("Unsupported value");
    }
}

void motionMoveSteps(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    // Get unit vector for movement
    float val = floatFromAny(resolveValue(target, block->inputs["STEPS"])).value();
    float dir = target->getDirection();
    glm::vec2 vec = glm::vec2(std::cos(DEG2RAD * (90 - dir)), std::sin(DEG2RAD * (90 - dir)));
    vec *= val;

    glm::vec2 currentPos = target->getPosition();
    currentPos += vec;
    target->setPosition(currentPos);
}

void motionTurnLeft(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float val = floatFromAny(resolveValue(target, block->inputs["DEGREES"])).value();

    int ang = target->getDirection();
    ang -= val;
    target->setDirection(ang);
}

void motionTurnRight(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float val = floatFromAny(resolveValue(target, block->inputs["DEGREES"])).value();

    float ang = target->getDirection();
    ang += val;
    target->setDirection(ang);
}

void motionGoToXY(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float x = floatFromAny(resolveValue(target, block->inputs["X"])).value();
    float y = floatFromAny(resolveValue(target, block->inputs["Y"])).value();

    target->setPosition(glm::ivec2(x, y));
}

void motionPointInDirection(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float val = floatFromAny(resolveValue(target, block->inputs["DIRECTION"])).value();
    target->setDirection(val);
}

void controlIf(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    // Get the value of the condition
    std::shared_ptr<ScratchBlock> condition = 
        std::any_cast<std::shared_ptr<ScratchBlock>>(block->inputs["CONDITION"]);

    std::shared_ptr<ScratchBlock> substack = 
        std::any_cast<std::shared_ptr<ScratchBlock>>(block->inputs["SUBSTACK"]);

    auto val = getValueOfReporterBlock(target, condition);

    if (std::any_cast<int>(val) != 0) {
        ExecBlock(target, substack);
    }
}

void controlElseIf(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    // Get the value of the condition
    std::shared_ptr<ScratchBlock> condition = 
        std::any_cast<std::shared_ptr<ScratchBlock>>(block->inputs["CONDITION"]);

    std::shared_ptr<ScratchBlock> substack = 
        std::any_cast<std::shared_ptr<ScratchBlock>>(block->inputs["SUBSTACK"]);

    std::shared_ptr<ScratchBlock> substack2 = 
        std::any_cast<std::shared_ptr<ScratchBlock>>(block->inputs["SUBSTACK2"]);

    auto val = getValueOfReporterBlock(target, condition);

    if (std::any_cast<int>(val) != 0) {
        ExecBlock(target, substack);
    } else {
        ExecBlock(target, substack2);
    }
}

std::any operatorGt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    // Try converting both values to numbers
    std::optional<float> fparam1 = floatFromAny(param1);
    std::optional<float> fparam2 = floatFromAny(param2);

    // If both are numbers, compare numerically
    if (fparam1 && fparam2) {
        return static_cast<int>(*fparam1 > *fparam2);
    }

    // Otherwise, compare as strings
    std::string sparam1 = stringFromAny(param1);
    std::string sparam2 = stringFromAny(param2);

    return static_cast<int>(sparam1 > sparam2);
}

std::any operatorLt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    // Try converting both values to numbers
    std::optional<float> fparam1 = floatFromAny(param1);
    std::optional<float> fparam2 = floatFromAny(param2);

    // If both are numbers, compare numerically
    if (fparam1 && fparam2) {
        return static_cast<int>(*fparam1 < *fparam2);
    }

    // Otherwise, compare as strings
    std::string sparam1 = stringFromAny(param1);
    std::string sparam2 = stringFromAny(param2);

    return static_cast<int>(sparam1 < sparam2);
}