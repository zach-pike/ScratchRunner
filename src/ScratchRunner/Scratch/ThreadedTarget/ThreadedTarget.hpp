#pragma once


#include "ScratchRunner/Scratch/ScratchBlock.hpp"
#include "ScratchRunner/Scratch/ScratchCostume.hpp"
#include "ScratchRunner/Utility/Vars.hpp"

#include "glm/glm.hpp"
#include "GL/ShaderProgram/ShaderProgram.hpp"

#include "Worker/Worker.hpp"

#include <thread>
#include <shared_mutex>
#include <mutex>
#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <random>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

struct TargetUniforms {
    GLuint windowSize;
    GLuint imageSize;
    GLuint position;
    GLuint scale;
    GLuint aspect;
    GLuint costumeSamp;
    GLuint angle;
};

class Runner;

class ThreadedTarget {
private:
    Runner* runnerParent;

    const bool stage;
    const std::string name;

    mutable std::shared_mutex variablesLock;
    std::map<std::string, ScratchValue> variables;

    mutable std::shared_mutex listsLock;
    std::map<std::string, std::vector<ScratchValue>> lists;

    // Broadcasts WIP
    
    // This should never be modified
    const std::vector<std::shared_ptr<ScratchBlock>> blocks;

    mutable std::shared_mutex currentCostumeLock;
    int currentCostume;

    const std::vector<std::shared_ptr<ScratchCostume>> costumes;

    mutable std::shared_mutex layerOrderLock;
    int layerOrder;

    mutable std::shared_mutex visibleLock;
    bool visible;

    mutable std::shared_mutex positionLock;
    glm::dvec2 position;

    mutable std::shared_mutex sizeLock;
    double size;

    mutable std::shared_mutex directionLock;
    double direction;

    mutable std::shared_mutex draggableLock;
    bool draggable;

    std::vector<std::shared_ptr<Worker>> workers;
    std::map<std::string, std::vector<std::shared_ptr<ScratchBlock>>> eventHandlers;

    mutable std::mutex rngLock;
    mutable std::mt19937 rng;
    
    // OpenGL stuff
    std::vector<GLuint> costumeTextures;
public:
    // Getters
    bool isStage() const;
    std::string getName() const;
    std::optional<ScratchValue> getVariable(std::string id) const;
    bool hasVariable(std::string id) const;
    bool hasList(std::string id) const;
    std::optional<std::vector<ScratchValue>> getList(std::string id) const;
    std::vector<std::shared_ptr<ScratchBlock>> getBlocks() const;
    int getCurrentCostumeID() const;
    std::shared_ptr<ScratchCostume> getCurrentCostume() const;
    std::vector<std::shared_ptr<ScratchCostume>> getCostumes() const;
    int getLayerOrder() const;
    bool getVisible() const;
    glm::dvec2 getPosition() const;
    double getSize() const;
    double getDirection() const;
    bool isDraggable() const;

    int randomInt(int min, int max) const;
    double randomDouble(double min, double max) const;

    Runner* getRunnerParent() const;
    std::shared_ptr<ThreadedTarget> getStage() const;

    // Setters
    void setVariable(std::string id, ScratchValue value);
    void setList(std::string id, std::vector<ScratchValue> value);

    // List operations (saves on memory moving)
    void listAppend(std::string id, ScratchValue value);
    void listDeleteItem(std::string id, int scratchIndex);
    void listClear(std::string id);
    void listInsertAt(std::string id, int scratchIndex, ScratchValue value);
    void listReplaceAt(std::string id, int scratchIndex, ScratchValue value);

    ScratchValue listAt(std::string id, int scratchIndex) const;
    std::size_t listFind(std::string id, ScratchValue value) const;
    std::size_t listLength(std::string id) const;
    bool listContains(std::string id, ScratchValue value) const;

    void setCurrentCostumeID(int costume);
    void setLayerOrder(int layerOrder);
    void setVisible(bool visible);
    void setPosition(glm::vec2 pos);
    void setSize(double size);
    void setDirection(double direction);
    void setDraggable(bool draggable);

    void evalTopLevelBlocks();
    void processEvent(std::string event);
public:
    ThreadedTarget(ThreadedTarget&) = delete;
    ThreadedTarget(ThreadedTarget&&) = delete;
    void operator=(ThreadedTarget&) = delete;

    ThreadedTarget(
        Runner* runner,
        bool isStage,
        std::string name,
        std::map<std::string, ScratchValue> variables,
        std::map<std::string, std::vector<ScratchValue>> lists,
        std::vector<std::shared_ptr<ScratchBlock>> blocks,
        int currentCostume,
        std::vector<std::shared_ptr<ScratchCostume>> costumes,
        int layerOrder,
        bool visible,
        glm::dvec2 position,
        double size,
        double direction,
        bool draggable
    );

    ~ThreadedTarget();

    void initCostumeTextures();
    void uninitCostumeTextures();

    void drawTarget(std::shared_ptr<ShaderProgram> program, const TargetUniforms& uniforms, glm::vec2 windowSize) const;
};