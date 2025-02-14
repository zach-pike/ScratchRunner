#include "ThreadedTarget.hpp"

#include "Exec/Exec.hpp"

#include <stdexcept>
#include <exception>
#include <iostream>
#include <functional>

#include "../../Runner.hpp"

bool ThreadedTarget::isStage() const { return stage; }
std::string ThreadedTarget::getName() const { return name; }

std::optional<std::any> ThreadedTarget::getVariable(std::string id) const {
    std::shared_lock lock(variablesLock);

    // Try and find the var
    auto it = variables.find(id);

    if (it == variables.end())
        return std::nullopt;
    else
        return it->second;
}

std::optional<std::vector<std::any>> ThreadedTarget::getList(std::string id) const {
    std::shared_lock lock(listsLock);

    // Try and find the var
    auto it = lists.find(id);

    if (it == lists.end())
        return std::nullopt;
    else
        return it->second;
}

std::vector<std::shared_ptr<ScratchBlock>> ThreadedTarget::getBlocks() const { return blocks; }

int ThreadedTarget::getCurrentCostumeID() const {
    std::shared_lock lock(currentCostumeLock);
    return currentCostume;
}

std::shared_ptr<ScratchCostume> ThreadedTarget::getCurrentCostume() const {
    return costumes.at(getCurrentCostumeID());
}

std::vector<std::shared_ptr<ScratchCostume>> ThreadedTarget::getCostumes() const { return costumes; }

int ThreadedTarget::getLayerOrder() const {
    std::shared_lock lock(layerOrderLock);
    return layerOrder;
}

bool ThreadedTarget::getVisible() const {
    std::shared_lock lock(visibleLock);
    return visible;
}

glm::dvec2 ThreadedTarget::getPosition() const {
    std::shared_lock lock(positionLock);
    return position;
}

double ThreadedTarget::getSize() const {
    std::shared_lock lock(sizeLock);
    return size;
}

double ThreadedTarget::getDirection() const {
    std::shared_lock lock(directionLock);
    return direction;
}

bool ThreadedTarget::isDraggable() const {
    std::shared_lock lock(draggableLock);
    return draggable;
}

int ThreadedTarget::randomInt(int min, int max) const {
    rngLock.lock();
    std::uniform_int_distribution<int> dist(min, max);
    int v = dist(rng);
    rngLock.unlock();
    return v;
}

double ThreadedTarget::randomDouble(double min, double max) const {
    rngLock.lock();
    std::uniform_real_distribution<double> dist(min, max);
    double v = dist(rng);
    rngLock.unlock();
    return v;
}

Runner* ThreadedTarget::getRunnerParent() const {
    return runnerParent;
}

// Setters
void ThreadedTarget::setVariable(std::string id, std::any value) {
    std::unique_lock lock(variablesLock);

    assert(variables.count(id) > 0);

    variables.at(id) = value;
}

void ThreadedTarget::setList(std::string id, std::vector<std::any> value) {
    std::unique_lock lock(listsLock);

    assert(lists.count(id) > 0);

    lists.at(id) = value;
}

void ThreadedTarget::setCurrentCostumeID(int costume) {
    std::unique_lock lock(currentCostumeLock);
    currentCostume = costume;
}

void ThreadedTarget::setLayerOrder(int _layerOrder) {
    std::unique_lock lock(layerOrderLock);
    layerOrder = _layerOrder;
}

void ThreadedTarget::setVisible(bool _visible) {
    std::unique_lock lock(visibleLock);
    visible = _visible;
}

void ThreadedTarget::setPosition(glm::vec2 pos) {
    std::unique_lock lock(positionLock);
    position = pos;
}

void ThreadedTarget::setSize(double _size) {
    std::unique_lock lock(sizeLock);
    size = _size;
}

void ThreadedTarget::setDirection(double _direction) {
    std::unique_lock lock(directionLock);
    direction = _direction;
}

void ThreadedTarget::setDraggable(bool _draggable) {
    std::unique_lock lock(draggableLock);
    draggable = _draggable;
}

void ThreadedTarget::evalTopLevelBlocks() {
    for (auto& block : blocks) {
        int a = block->opcode.find('_');
        std::string nsp = block->opcode.substr(0, a);
        std::string type = block->opcode.substr(a+1);

        if (nsp == "event" && block->next.has_value()) {
            eventHandlers[type].push_back(block->next.value());
        } else {
            throw std::runtime_error("Invalid top level block type!");
        }
    }
}

void ThreadedTarget::processEvent(std::string event) {
    auto assignWork = [&](std::function<void()> eventFunc) {
        for (auto& worker : workers) {
            if (!worker->isCurrentlyWorking()) {
                worker->assignWork(eventFunc);
                return;
            }
        }

        // If no worker was available, create a new one
        auto newWorker = std::make_shared<Worker>();
        newWorker->assignWork(eventFunc);

        workers.push_back(newWorker);
    };

    if (eventHandlers.contains(event)) {
        for (auto& handler : eventHandlers[event]) {
            auto func = [this, &handler]() {
                ExecBlock(this, handler);
            };

            assignWork(func);
        }
    }
}

ThreadedTarget::ThreadedTarget(
    Runner* _runner,
    bool _isStage,
    std::string _name,
    std::map<std::string, std::any> _variables,
    std::map<std::string, std::vector<std::any>> _lists,
    std::vector<std::shared_ptr<ScratchBlock>> _blocks,
    int _currentCostume,
    std::vector<std::shared_ptr<ScratchCostume>> _costumes,
    int _layerOrder,
    bool _visible,
    glm::dvec2 _targetPosition,
    double _size,
    double _direction,
    bool _draggable
):
    runnerParent(_runner),
    stage(_isStage),
    name(_name),
    variables(_variables),
    lists(_lists),
    blocks(_blocks),
    costumes(_costumes),
    currentCostume(_currentCostume),
    layerOrder(_layerOrder),
    visible(_visible),
    position(_targetPosition),
    size(_size),
    direction(_direction),
    draggable(_draggable)
{
    
}

ThreadedTarget::~ThreadedTarget() {
    
}

void ThreadedTarget::initCostumeTextures() {
    for (auto& costume : getCostumes()) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering (minification & magnification filters)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            costume->pixelWidth,
            costume->pixelHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            costume->pixels.data()
        );

        costumeTextures.push_back(texture);
    }
}

void ThreadedTarget::uninitCostumeTextures() {
    for (GLuint& texture : costumeTextures) {
        glDeleteTextures(1, &texture);
    }

    costumeTextures.clear();
}

void ThreadedTarget::drawTarget(std::shared_ptr<ShaderProgram> program, const TargetUniforms& uniforms, glm::vec2 windowSize) const {
    program->use();
    GLuint textureID = costumeTextures.at(getCurrentCostumeID());
    
    glActiveTexture(GL_TEXTURE0);  // Activate the first texture unit
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(uniforms.costumeSamp, 0);

    // Calculate aspect ratio of the sprite
    auto costume = getCurrentCostume();
    float aspect = (float) costume->pixelWidth / costume->pixelHeight;
    float scale = (float) getSize() / 100.f;
    float angle = getDirection();

    glm::vec2 pos = getPosition();
    glm::vec2 imgSize(costume->pixelWidth, costume->pixelHeight);

    glUniform2fv(uniforms.windowSize, 1, &windowSize[0]);
    glUniform2fv(uniforms.imageSize, 1, &imgSize[0]);
    glUniform2fv(uniforms.position, 1, &pos[0]);

    glUniform1f(uniforms.aspect, aspect);
    glUniform1f(uniforms.scale, scale);
    glUniform1f(uniforms.angle, angle);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}