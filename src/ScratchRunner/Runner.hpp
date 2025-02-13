#pragma once

#include <memory>
#include <filesystem>

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "GL/ShaderLoaders/VertexFragment.hpp"

#include "Scratch/ThreadedTarget/ThreadedTarget.hpp"

namespace fs = std::filesystem;

struct Variable {
    std::string id;
};

class Runner {
private:
    GLFWwindow* window;

    void initOpenGL();
    void uninitOpenGL();

    std::shared_ptr<ShaderProgram> targetShaderProgram;
    TargetUniforms targetUniforms;

    void createOpenGLRenderObjects();
    void destroyOpenGLRenderObjects();

    bool targetsSorted = false;
    std::vector<std::shared_ptr<ThreadedTarget>> targets;

    void drawTargets(glm::vec2 windowSize);

    void mainLoop();
private:
    void broadcastEvent(std::string s);
public:
    Runner() = default;
    ~Runner() = default;

    void loadProject(fs::path basePath);

    void run();
};