#include "Runner.hpp"

#include <iostream>
#include <chrono>
#include <thread>

void Runner::initOpenGL() {
    // Initialize OpenGL and load relevant extensions
    if (!glfwInit()) {
        throw std::runtime_error("Failed to init glfw!");
    }

    window = glfwCreateWindow(960, 720, "Window", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load OpenGL/GLAD!\n";
        std::exit(1);
    }

    glClearColor(1.f, 1.f, 1.f, 1.0f);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

void Runner::uninitOpenGL() {
    glfwTerminate();
}

void Runner::createOpenGLRenderObjects() {
    // Create opengl textures for the costumes in each target
    for (auto& target : targets) {
        target->initCostumeTextures();
    }

    targetShaderProgram = loadVertexFragmentShader("shaders/target/");
    targetShaderProgram->use();

    targetUniforms.windowSize = targetShaderProgram->getUniformLocation("windowSize");
    targetUniforms.imageSize =  targetShaderProgram->getUniformLocation("imageSize");
    targetUniforms.position = targetShaderProgram->getUniformLocation("position");
    targetUniforms.scale = targetShaderProgram->getUniformLocation("scale");
    targetUniforms.aspect = targetShaderProgram->getUniformLocation("aspect");
    targetUniforms.costumeSamp = targetShaderProgram->getUniformLocation("costumeTexture");
    targetUniforms.angle = targetShaderProgram->getUniformLocation("angle");
}

void Runner::destroyOpenGLRenderObjects() {
    for (auto& target : targets) {
        target->uninitCostumeTextures();
    }

    targetShaderProgram.reset();
}

void Runner::drawTargets(glm::vec2 windowSize) {
    if (!targetsSorted) {
        std::sort(
            targets.begin(),
            targets.end(),
            [](std::shared_ptr<ThreadedTarget> a, std::shared_ptr<ThreadedTarget> b) {
                return a->getLayerOrder() < b->getLayerOrder();
            }
        );

        targetsSorted = true;
    }

    for (auto& target : targets) {
        target->drawTarget(targetShaderProgram, targetUniforms, windowSize);
    }
}

void Runner::mainLoop() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::chrono::time_point tp1 = std::chrono::steady_clock::now();
    std::chrono::time_point tp2 = std::chrono::steady_clock::now();

    float aspectRatio;

    for (auto& target : targets) {
        target->evalTopLevelBlocks();
    }

    broadcastEvent("whenflagclicked");

    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto frameBegin = std::chrono::steady_clock::now();
        double dt = (tp2 - tp1).count() / (double) (1e6 * 16);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        if (windowWidth != 0 && windowHeight != 0) {
            aspectRatio = (float)windowWidth / (float)windowHeight;
        }

        // Render code here

        drawTargets(glm::vec2(windowWidth, windowHeight));

        // End render code
        
        glfwSwapBuffers(window);

        glViewport(0, 0, windowWidth, windowHeight);

        auto frameEnd = std::chrono::steady_clock::now();
        std::int64_t diff = (frameEnd - frameBegin).count();

        tp1 = tp2;
        tp2 = frameEnd;

        // If frame took less than 16ms to complete than sleep for rest of time
        if (diff < (1e6 * 14)) {
            std::int64_t sleepTime = (1e6 * 14) - diff;
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
        }
    }

}

void Runner::broadcastEvent(std::string s) {
    for (auto& target : targets) {
        target->processEvent(s);
    }
}

void Runner::loadProject(std::shared_ptr<ScratchProject> _project) {
    originalProject = _project;

    for (auto& target : originalProject->targets) {
        auto a = std::make_shared<ThreadedTarget>(
            target->isStage,
            target->name,
            target->variables,
            target->lists,
            target->blocks,
            target->currentCostume,
            target->costumes,
            target->layerOrder,
            target->visible,
            glm::vec2(target->posX, target->posY),
            target->size,
            target->direction,
            target->draggable
        );

        targets.push_back(a);
    }
}

void Runner::run() {
    initOpenGL();
    createOpenGLRenderObjects();

    mainLoop();

    destroyOpenGLRenderObjects();
    uninitOpenGL();
}