#include "Handlers.hpp"

#include "ScratchRunner/Utility/Conv.hpp"
#include "ScratchRunner/Utility/Vars.hpp"
#include "../Exec.hpp"
#include "ScratchRunner/Runner.hpp"
#include <cxxabi.h>
#include <iostream>
#include <algorithm>

#define DEG2RAD (M_PI / 180.f)

static std::string demangle(const char* name) {
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = (status == 0) ? demangled : name;
    free(demangled);
    return result;
}

static ScratchValue resolveValue(ThreadedTarget* target, ScratchValue v) {
    if (std::holds_alternative<double>(v) ||
        std::holds_alternative<std::string>(v)
    ) {
        return v;
    } else if (std::holds_alternative<Variable>(v)) {
        Variable var = std::get<Variable>(v);
        auto val = target->getVariable(var.id);

        if (!val.has_value()) {
            val = target->getStage()->getVariable(var.id);
        }

        assert(val.has_value());

        return val.value();
    } else if (std::holds_alternative<List>(v)) {
        List var = std::get<List>(v);
        auto val = target->getList(var.id);

        if (!val.has_value()) {
            val = target->getStage()->getList(var.id);
        }

        assert(val.has_value());

        auto& list = val.value();

        // Do the stupid thing scratch does
        auto stringIt = std::find_if(list.begin(), list.end(), [](ScratchValue& a) {
            return !isValidDouble(stringFromAny(a));
        });

        bool hasSpaces = stringIt != list.end();

        std::string s;

        for (int i=0; i<list.size(); i++) {
            s += stringFromAny(list.at(i));
            if (hasSpaces && i != (list.size() - 1)) s += ' ';
        }

        return s;
    } else if (std::holds_alternative<std::shared_ptr<ScratchBlock>>(v)) {
        auto a = std::get<std::shared_ptr<ScratchBlock>>(v);

        return getValueOfReporterBlock(target, a);
    } else {
        assert(false);
    }
}

void motionMoveSteps(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    // Get unit vector for movement
    double val = doubleFromAny(resolveValue(target, block->inputs["STEPS"]));
    double dir = target->getDirection();
    glm::vec2 vec = glm::vec2(std::cos(DEG2RAD * (90 - dir)), std::sin(DEG2RAD * (90 - dir)));
    vec *= val;

    glm::dvec2 currentPos = target->getPosition();
    currentPos += vec;
    target->setPosition(currentPos);
}

void motionTurnLeft(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double val = doubleFromAny(resolveValue(target, block->inputs["DEGREES"]));

    double ang = target->getDirection();
    ang -= val;
    target->setDirection(ang);
}

void motionTurnRight(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double val = doubleFromAny(resolveValue(target, block->inputs["DEGREES"]));

    double ang = target->getDirection();
    ang += val;
    target->setDirection(ang);
}

void motionGoToXY(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double x = doubleFromAny(resolveValue(target, block->inputs["X"]));
    double y = doubleFromAny(resolveValue(target, block->inputs["Y"]));

    target->setPosition(glm::ivec2(x, y));
}

void motionPointInDirection(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    double val = doubleFromAny(resolveValue(target, block->inputs["DIRECTION"]));
    target->setDirection(val);
}

void controlIf(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    if (!block->inputs.contains("CONDITION") || !block->inputs.contains("SUBSTACK")) return;

    // Get the value of the condition
    std::shared_ptr<ScratchBlock> condition = 
        std::get<std::shared_ptr<ScratchBlock>>(block->inputs["CONDITION"]);

    std::shared_ptr<ScratchBlock> substack = 
        std::get<std::shared_ptr<ScratchBlock>>(block->inputs["SUBSTACK"]);

    auto val = getValueOfReporterBlock(target, condition);

    if (std::get<bool>(val) != 0) {
        ExecBlock(target, substack);
    }
}

void controlIfElse(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    if (!block->inputs.contains("CONDITION")) return;

    // Get the value of the condition
    std::shared_ptr<ScratchBlock> condition = 
        std::get<std::shared_ptr<ScratchBlock>>(block->inputs["CONDITION"]);

    auto val = getValueOfReporterBlock(target, condition);

    if (std::get<bool>(val) != 0) {
        if (!block->inputs.contains("SUBSTACK")) return;
        std::shared_ptr<ScratchBlock> substack = 
            std::get<std::shared_ptr<ScratchBlock>>(block->inputs["SUBSTACK"]);
        ExecBlock(target, substack);
    } else {
        if (!block->inputs.contains("SUBSTACK2")) return;
        
        std::shared_ptr<ScratchBlock> substack2 = 
            std::get<std::shared_ptr<ScratchBlock>>(block->inputs["SUBSTACK2"]);

        ExecBlock(target, substack2);
    }
}

ScratchValue operatorAdd(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    return fparam1 + fparam2;
}

ScratchValue operatorSubtract(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    return fparam1 - fparam2;
}

ScratchValue operatorMultiply(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    return fparam1 * fparam2;
}

ScratchValue operatorDivide(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["NUM1"]);
    auto param2 = resolveValue(target, block->inputs["NUM2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    return fparam1 / fparam2;
}

ScratchValue operatorRandom(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto from = doubleFromAny(resolveValue(target, block->inputs["FROM"]));
    auto to = doubleFromAny(resolveValue(target, block->inputs["TO"]));

    if (isNotExactInteger(from) || isNotExactInteger(to)) {
        double d = target->randomDouble(from, to);
        return d;
    } else {
        double d = target->randomInt(from, to);
        return d;
    }
}

ScratchValue operatorGt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    auto fparam1 = doubleFromAny(param1);
    auto fparam2 = doubleFromAny(param2);

    return static_cast<bool>(fparam1 > fparam2);

    std::string sparam1 = stringFromAny(param1);
    std::string sparam2 = stringFromAny(param2);

    return static_cast<bool>(sparam1 > sparam2);
}

ScratchValue operatorLt(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    // Try converting both values to numbers
    std::optional<double> fparam1 = doubleFromAny(param1);
    std::optional<double> fparam2 = doubleFromAny(param2);

    if (fparam1 && fparam2) {
        return static_cast<bool>(*fparam1 < *fparam2);
    }

    std::string sparam1 = stringFromAny(param1);
    std::string sparam2 = stringFromAny(param2);

    return static_cast<bool>(sparam1 < sparam2);
}

ScratchValue operatorEquals(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    return static_cast<bool>(valuesAreEqual(param1, param2));
}

ScratchValue operatorAnd(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    bool val1 = std::get<bool>(param1);
    bool val2 = std::get<bool>(param2);

    return static_cast<bool>(val1 && val2);
}

ScratchValue operatorOr(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND1"]);
    auto param2 = resolveValue(target, block->inputs["OPERAND2"]);

    bool val1 = std::get<bool>(param1);
    bool val2 = std::get<bool>(param2);

    return static_cast<bool>(val1 || val2);
}

ScratchValue operatorNot(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto param1 = resolveValue(target, block->inputs["OPERAND"]);

    return static_cast<bool>(!std::get<bool>(param1));
}

void dataSetVariableTo(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto value = resolveValue(target, block->inputs.at("VALUE"));
    std::string variableID = block->fields.at("VARIABLE");

    // Try and see if its a sprite var
    if (target->hasVariable(variableID)) {
        target->setVariable(variableID, value);
    } else if (target->getStage()->hasVariable(variableID)) {
        target->getStage()->setVariable(variableID, value);
    } else {
        assert(false);
    }
}

void dataChangeVariableBy(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto value = resolveValue(target, block->inputs.at("VALUE"));
    std::string variableID = block->fields.at("VARIABLE");

    // Is this a local var or a global var?
    if (target->hasVariable(variableID)) {
        auto currentVal = target->getVariable(variableID).value();

        if (std::holds_alternative<std::string>(currentVal)) {
            target->setVariable(variableID, doubleFromAny(value));
        } else if (std::holds_alternative<double>(currentVal)) {
            double newValue = std::get<double>(currentVal) + doubleFromAny(value);
            target->setVariable(variableID, newValue);
        } else {
            assert(false);
        }

    } else if (target->getStage()->hasVariable(variableID)) {
        auto stage = target->getStage();

        auto currentVal = stage->getVariable(variableID).value();

        if (std::holds_alternative<std::string>(currentVal)) {
            stage->setVariable(variableID, doubleFromAny(value));
        } else if (std::holds_alternative<double>(currentVal)) {
            double newValue = std::get<double>(currentVal) + doubleFromAny(value);
            stage->setVariable(variableID, newValue);
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
}

void dataAddToList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    auto value = resolveValue(target, block->inputs.at("ITEM"));
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        target->listAppend(listID, value);
    } else if (target->getStage()->hasList(listID)) {
        target->getStage()->listAppend(listID, value);
    } else {
        assert(false);
    }
}

void dataDeleteOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    int index = std::floor(doubleFromAny(resolveValue(target, block->inputs.at("INDEX"))));
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        target->listDeleteItem(listID, index);
    } else if (target->getStage()->hasList(listID)) {
        target->getStage()->listDeleteItem(listID, index);
    } else {
        assert(false);
    }
}

void dataDeleteAllOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    std::string listID = block->fields.at("LIST");
    if (target->hasList(listID)) {
        target->listClear(listID);
    } else if (target->getStage()->hasList(listID)) {
        target->getStage()->listClear(listID);
    } else {
        assert(false);
    }
}

void dataInsertAtList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    int index = std::floor(doubleFromAny(resolveValue(target, block->inputs.at("INDEX"))));
    auto item = resolveValue(target, block->inputs.at("ITEM"));
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        target->listInsertAt(listID, index, item);
    } else if (target->getStage()->hasList(listID)) {
        target->getStage()->listInsertAt(listID, index, item);
    } else {
        assert(false);
    }
}

void dataReplaceItemOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    int index = std::floor(doubleFromAny(resolveValue(target, block->inputs.at("INDEX"))));
    auto item = resolveValue(target, block->inputs.at("ITEM"));
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        target->listReplaceAt(listID, index, item);
    } else if (target->getStage()->hasList(listID)) {
        target->getStage()->listReplaceAt(listID, index, item);
    } else {
        assert(false);
    }
}

ScratchValue dataItemOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    int index = std::floor(doubleFromAny(resolveValue(target, block->inputs.at("INDEX"))));
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        return target->listAt(listID, index);
    } else if (target->getStage()->hasList(listID)) {
        return target->getStage()->listAt(listID, index);
    } else {
        assert(false);
    }
}

ScratchValue dataItemNumOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    ScratchValue item = resolveValue(target, block->inputs.at("ITEM"));
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        return static_cast<double>(target->listFind(listID, item));
    } else if (target->getStage()->hasList(listID)) {
        return static_cast<double>(target->getStage()->listFind(listID, item));
    } else {
        assert(false);
    }
}

ScratchValue dataLengthOfList(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    std::string listID = block->fields.at("LIST");

    if (target->hasList(listID)) {
        return static_cast<double>(target->listLength(listID));
    } else if (target->getStage()->hasList(listID)) {
        return static_cast<double>(target->getStage()->listLength(listID));
    } else {
        assert(false);
    }
}

ScratchValue dataListContainsItem(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    std::string listID = block->fields.at("LIST");
    ScratchValue item = resolveValue(target, block->inputs.at("ITEM"));

    if (target->hasList(listID)) {
        return static_cast<bool>(target->listContains(listID, item));
    } else if (target->getStage()->hasList(listID)) {
        return static_cast<bool>(target->getStage()->listContains(listID, item));
    } else {
        assert(false);
    }
}

void debugPrint(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    ScratchValue item = resolveValue(target, block->inputs.at("MESSAGE"));

    std::cout << "DEBUG: \"" << stringFromAny(item) << "\"\n";
}