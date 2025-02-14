#include "Handlers.hpp"

#include "ScratchRunner/Utility/Conv.hpp"
#include "../Exec.hpp"
#include "ScratchRunner/Runner.hpp"

#include <iostream>


#define DEG2RAD (M_PI / 180.f)

static std::any resolveValue(ThreadedTarget* target, std::any v) {
    if (v.type() == typeid(double) ||
        v.type() == typeid(std::string)
    ) {
        return v;
    } else if (v.type() == typeid(Variable)) {
        Variable var = std::any_cast<Variable>(v);

        
        auto val = target->getVariable(var.id);

        if (!val.has_value()) {
            val = target->getRunnerParent()->getStage()->getVariable(var.id);
        }

        assert(val.has_value());

        return resolveValue(target, val.value());
    } else if (v.type() == typeid(std::shared_ptr<ScratchBlock>)) {
        auto a = std::any_cast<std::shared_ptr<ScratchBlock>>(v);

        return getValueOfReporterBlock(target, a);
    } else {
        throw std::runtime_error("Unsupported value" + std::string(v.type().name()));
    }
}

static bool isNotExactInteger(double value) {
    return std::floor(value) != value;
}

void motionMoveSteps(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    // Get unit vector for movement
    double val = doubleFromAny(resolveValue(target, block->inputs["STEPS"])).value();
    double dir = target->getDirection();
    glm::vec2 vec = glm::vec2(std::cos(DEG2RAD * (90 - dir)), std::sin(DEG2RAD * (90 - dir)));
    vec *= val;

    glm::dvec2 currentPos = target->getPosition();
    currentPos += vec;
    target->setPosition(currentPos);
}

void motionTurnLeft(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double val = doubleFromAny(resolveValue(target, block->inputs["DEGREES"])).value();

    int ang = target->getDirection();
    ang -= val;
    target->setDirection(ang);
}

void motionTurnRight(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double val = doubleFromAny(resolveValue(target, block->inputs["DEGREES"])).value();

    double ang = target->getDirection();
    ang += val;
    target->setDirection(ang);
}

void motionGoToXY(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double x = doubleFromAny(resolveValue(target, block->inputs["X"])).value();
    double y = doubleFromAny(resolveValue(target, block->inputs["Y"])).value();

    target->setPosition(glm::ivec2(x, y));
}

void motionPointInDirection(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double val = doubleFromAny(resolveValue(target, block->inputs["DIRECTION"])).value();
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

std::any operatorAdd(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    if (!fparam1 || !fparam2)
        throw std::runtime_error("Cannot add given types");

    return *fparam1 + *fparam2;
}

std::any operatorSubtract(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    if (!fparam1 || !fparam2)
        throw std::runtime_error("Cannot subtract given types");

    return *fparam1 - *fparam2;
}

std::any operatorMultiply(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    if (!fparam1 || !fparam2)
        throw std::runtime_error("Cannot multiply given types");

    return *fparam1 * *fparam2;
}

std::any operatorDivide(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    if (!fparam1 || !fparam2)
        throw std::runtime_error("Cannot divide given types");

    return *fparam1 / *fparam2;
}

std::any operatorGt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    // Try converting both values to numbers
    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

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
    std::optional<double> fparam1 = doubleFromAny(param1);
    std::optional<double> fparam2 = doubleFromAny(param2);

    // If both are numbers, compare numerically
    if (fparam1 && fparam2) {
        return static_cast<int>(*fparam1 < *fparam2);
    }

    // Otherwise, compare as strings
    std::string sparam1 = stringFromAny(param1);
    std::string sparam2 = stringFromAny(param2);

    return static_cast<int>(sparam1 < sparam2);
}

std::any operatorRandom(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto from = doubleFromAny(resolveValue(target, block->inputs["FROM"])).value();
    auto to = doubleFromAny(resolveValue(target, block->inputs["TO"])).value();


    if (isNotExactInteger(from) || isNotExactInteger(to)) {
        double d = target->randomDouble(from, to);
        return d;
    } else {
        double d = target->randomInt(from, to);
        return d;
    }
}