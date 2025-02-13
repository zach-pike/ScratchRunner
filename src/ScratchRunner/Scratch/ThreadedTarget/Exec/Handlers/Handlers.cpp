#include "Handlers.hpp"

#include "ScratchRunner/Utility/Conv.hpp"

#define DEG2RAD (M_PI / 180.f)

void motionMoveSteps(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    // Get unit vector for movement
    float val = floatFromAny(target, block->inputs["STEPS"]);
    float dir = target->getDirection();
    glm::vec2 vec = glm::vec2(std::cos(DEG2RAD * (90 - dir)), std::sin(DEG2RAD * (90 - dir)));
    vec *= val;

    glm::vec2 currentPos = target->getPosition();
    currentPos += vec;
    target->setPosition(currentPos);
}

void motionTurnLeft(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float val = floatFromAny(target, block->inputs["DEGREES"]);

    int ang = target->getDirection();
    ang -= val;
    target->setDirection(ang);
}

void motionTurnRight(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float val = floatFromAny(target, block->inputs["DEGREES"]);

    float ang = target->getDirection();
    ang += val;
    target->setDirection(ang);
}

void motionGoToXY(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float x = floatFromAny(target, block->inputs["X"]);
    float y = floatFromAny(target, block->inputs["Y"]);

    target->setPosition(glm::ivec2(x, y));
}

void motionPointInDirection(ThreadedTarget* target, std::shared_ptr<ScratchBlock> block) {
    float val = floatFromAny(target, block->inputs["DIRECTION"]);

    target->setDirection(val);
}