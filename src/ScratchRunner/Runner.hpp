#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>
#include "Scratch/ScratchProject.hpp"
#include "GL/ShaderLoaders/VertexFragment.hpp"

#include "ThreadedTarget/ThreadedTarget.hpp"

class Runner {
private:
    GLFWwindow* window;
    std::shared_ptr<ScratchProject> originalProject;

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

    void loadProject(std::shared_ptr<ScratchProject> project);

    void run();
};