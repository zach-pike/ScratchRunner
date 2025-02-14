#pragma once


#include "ScratchRunner/Scratch/ScratchBlock.hpp"
#include "ScratchRunner/Scratch/ScratchCostume.hpp"

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

class ThreadedTarget {
private:
    const bool stage;
    const std::string name;

    mutable std::shared_mutex variablesLock;
    std::map<std::string, std::any> variables;

    mutable std::shared_mutex listsLock;
    std::map<std::string, std::vector<std::any>> lists;

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
    std::optional<std::any> getVariable(std::string id) const;
    std::optional<std::vector<std::any>> getList(std::string id) const;
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

    // Setters
    void setVariable(std::string id, std::any value);
    void setList(std::string id, std::vector<std::any> value);
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
        bool isStage,
        std::string name,
        std::map<std::string, std::any> variables,
        std::map<std::string, std::vector<std::any>> lists,
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