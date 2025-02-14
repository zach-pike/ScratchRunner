#include "Runner.hpp"

#include <iostream>
#include <chrono>
#include <thread>

#include "rapidjson/document.h"
#include "stb/stb_image.h"

#include <fstream>
#include <iostream>

#include <string>
#include <sstream>

#include <Zipper/unzipper.h>
using namespace zipper;

static std::vector<std::shared_ptr<ScratchCostume>> parseCostumesFromJSONNode(Unzipper& unzipper, rapidjson::Value& costumeNode) {
    std::vector<std::shared_ptr<ScratchCostume>> costumes;

    for (auto& costumeJson : costumeNode.GetArray()) {
        std::string name = costumeJson["name"].GetString();
        int bitmapRes = costumeJson["bitmapResolution"].GetInt();

        std::string filePath = std::string(costumeJson["md5ext"].GetString());

        int rotCenX = costumeJson["rotationCenterX"].GetInt();
        int rotCenY = costumeJson["rotationCenterY"].GetInt();

        std::vector<std::uint8_t> imageBytes;
        unzipper.extractEntryToMemory(filePath, imageBytes);

        int imgWidth, imgHeight;
        int channels;
        std::uint8_t* imgData = stbi_load_from_memory(imageBytes.data(), imageBytes.size(), &imgWidth, &imgHeight, &channels, 4);

        auto costume = std::make_shared<ScratchCostume>();
        costume->name = name;
        costume->bitmapRes = bitmapRes;
        costume->rotationCenterX = rotCenX;
        costume->rotationCenterY = rotCenY;
        costume->pixels = std::vector<std::uint8_t>(imgData, imgData + (imgWidth * imgHeight * channels));
        costume->pixelWidth = imgWidth;
        costume->pixelHeight = imgHeight;

        costumes.push_back(costume);

        stbi_image_free(imgData);
    }

    return std::move(costumes);
}

static std::vector<std::shared_ptr<ScratchBlock>> parseBlocksFromJSONNode(rapidjson::Value& blocksNode) {
    // Loop over the blocks and find all top level blocks
    std::vector<std::string> topLevelKeys;

    for (auto it = blocksNode.MemberBegin(); it != blocksNode.MemberEnd(); it ++) {
        if (it->value["topLevel"].GetBool()) {
            topLevelKeys.push_back(it->name.GetString());
        }
    }

    // Parse top level block into a tree structure
    using ParseBlockType = std::function<std::shared_ptr<ScratchBlock>(rapidjson::Value&, rapidjson::Value&)>;

    ParseBlockType parseBlock = [&](rapidjson::Value& blockRoot, rapidjson::Value& v) {
        auto newBlock = std::make_shared<ScratchBlock>();
        newBlock->opcode = v["opcode"].GetString();

        auto& inputs = v["inputs"];

        for (auto it = inputs.MemberBegin(); it != inputs.MemberEnd(); it++) {
            std::string inputName = it->name.GetString();

            const auto& inputData = it->value.GetArray()[1];
            
            if (inputData.IsArray()) {
                const auto& inputArray = inputData.GetArray();
                int inputType = inputArray[0].GetInt();

                if (inputType >= 4 && inputType <= 8) {
                    std::string v = inputArray[1].GetString();
                    newBlock->inputs[inputName] = std::stod(v);
                } else if (inputType >= 9 && inputType <= 10) {
                    std::string v = inputArray[1].GetString();
                    newBlock->inputs[inputName] = v;
                } else if (inputType == 12) {
                    std::string v = inputArray[2].GetString();
                    newBlock->inputs[inputName] = Variable{ v };
                } else {
                    throw std::runtime_error("Invalid input type!");
                }
            } else if (inputData.IsString()) {
                std::string v = inputData.GetString();

                // If its just a string then its a link to another block
                newBlock->inputs[inputName] = parseBlock(blockRoot, blockRoot[v.c_str()]);
            }
        }
                
        auto& nextJson = v["next"];
        if (!nextJson.IsNull()) {
            assert(blockRoot.HasMember(nextJson.GetString()));
            newBlock->next = parseBlock(blockRoot, blockRoot[nextJson.GetString()]);
        }

        return newBlock;
    };

    std::vector<std::shared_ptr<ScratchBlock>> blocks;
    for (std::string& key : topLevelKeys) {
        assert(blocksNode.HasMember(key.c_str()));
        blocks.push_back(parseBlock(blocksNode, blocksNode[key.c_str()]));
    }

    return std::move(blocks);
}

static std::vector<std::shared_ptr<ThreadedTarget>> parseTargetsFromJSONNode(Runner* runner, Unzipper& unzipper, rapidjson::Value& targetsJson) {
    std::vector<std::shared_ptr<ThreadedTarget>> targets;

    for (auto& targetJson : targetsJson.GetArray()) {
        bool isStage = targetJson["isStage"].GetBool();
        std::string name = targetJson["name"].GetString();

        // Parse the costumes
        auto costumes = parseCostumesFromJSONNode(unzipper, targetJson["costumes"]);

        std::map<std::string, std::any> variables;

        // Initialize all the variables
        auto& varsJson = targetJson["variables"];
        for (auto it = varsJson.MemberBegin(); it != varsJson.MemberEnd(); it++) {
            std::string varId = it->name.GetString();

            auto& val = it->value.GetArray()[1];
            auto type = val.GetType();

            if (type == rapidjson::Type::kNumberType) {
                variables[varId] = val.GetDouble();
            } else if (type == rapidjson::Type::kStringType) {
                variables[varId] = std::string(val.GetString());
            } else {
                assert("aaaaa");
            }
        }

        std::map<std::string, std::vector<std::any>> lists;

        // Parse lists WIP

        auto blocks = parseBlocksFromJSONNode(targetJson["blocks"]);

        int currentCostume = targetJson["currentCostume"].GetInt();
        double posX = targetJson.HasMember("x") ? targetJson["x"].GetDouble() : 0;
        double posY = targetJson.HasMember("y") ? targetJson["y"].GetDouble() : 0;
        double size = targetJson.HasMember("size") ? targetJson["size"].GetDouble() : 100;
        double direction = targetJson.HasMember("direction") ? targetJson["direction"].GetDouble() : 90;
        bool draggable = targetJson.HasMember("draggable") ? targetJson["draggable"].GetBool() : false;
        bool visible = targetJson.HasMember("visible") ? targetJson["visible"].GetBool() : true;
        int layerOrder = targetJson["layerOrder"].GetInt();

        auto target = std::make_shared<ThreadedTarget>(
            runner,
            isStage,
            name,
            variables,
            lists,
            blocks,
            currentCostume,
            costumes,
            layerOrder,
            visible,
            glm::dvec2(posX, posY),
            size,
            direction,
            draggable
        );
        
        targets.push_back(target);
    }

    return std::move(targets);
}

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
    {
        std::shared_lock lock(targetsListLock);
        for (auto& target : targets) {
            target->initCostumeTextures();
        }
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
    {
        std::shared_lock lock(targetsListLock);

        for (auto& target : targets) {
            target->uninitCostumeTextures();
        }
    }

    targetShaderProgram.reset();
}

void Runner::drawTargets(glm::vec2 windowSize) {
    if (!targetsSorted) {
        std::unique_lock lock(targetsListLock);
        std::sort(
            targets.begin(),
            targets.end(),
            [](std::shared_ptr<ThreadedTarget> a, std::shared_ptr<ThreadedTarget> b) {
                return a->getLayerOrder() < b->getLayerOrder();
            }
        );

        targetsSorted = true;
    }

    {
        std::shared_lock lock(targetsListLock);

        for (auto& target : targets) {
            target->drawTarget(targetShaderProgram, targetUniforms, windowSize);
        }
    }
}

void Runner::mainLoop() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::chrono::time_point tp1 = std::chrono::steady_clock::now();
    std::chrono::time_point tp2 = std::chrono::steady_clock::now();

    float aspectRatio;

    {
        std::shared_lock lock(targetsListLock);

        for (auto& target : targets) {
            target->evalTopLevelBlocks();
        }
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
    std::shared_lock lock(targetsListLock);

    for (auto& target : targets) {
        target->processEvent(s);
    }
}

std::shared_ptr<ThreadedTarget> Runner::getStage() const {
    return stage;
}

void Runner::loadProject(fs::path basePath) {
    Unzipper unzipper(basePath.string());
    std::vector<ZipEntry> entries = unzipper.entries();

    std::vector<std::uint8_t> projectJsonBytes;
    unzipper.extractEntryToMemory("project.json", projectJsonBytes);
    std::string fileString = std::string(projectJsonBytes.begin(), projectJsonBytes.end());

    rapidjson::Document document;
    document.Parse(fileString.c_str());

    std::unique_lock lock(targetsListLock);
    targets = parseTargetsFromJSONNode(this, unzipper, document["targets"]);

    stage = *std::find_if(targets.begin(), targets.end(), [](std::shared_ptr<ThreadedTarget>& a) { return a->isStage(); });

    unzipper.close();
}

void Runner::run() {
    initOpenGL();
    createOpenGLRenderObjects();

    mainLoop();

    destroyOpenGLRenderObjects();
    uninitOpenGL();
}