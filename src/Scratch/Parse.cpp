#include "Parse.hpp"

#include "rapidjson/document.h"
#include "stb/stb_image.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <functional>
#include <map>

static std::vector<std::shared_ptr<ScratchCostume>> parseCostumesFromJSONNode(rapidjson::Value& costumeNode) {
    std::vector<std::shared_ptr<ScratchCostume>> costumes;

    for (auto& costumeJson : costumeNode.GetArray()) {
        std::string name = costumeJson["name"].GetString();
        int bitmapRes = costumeJson["bitmapResolution"].GetInt();

        std::string filePath = "proj/" + std::string(costumeJson["md5ext"].GetString());

        int rotCenX = costumeJson["rotationCenterX"].GetInt();
        int rotCenY = costumeJson["rotationCenterY"].GetInt();

        int imgWidth, imgHeight;
        int channels;
        std::uint8_t* imgData = stbi_load(filePath.c_str(), &imgWidth, &imgHeight, &channels, 4);

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
            const auto& inputArray = inputData.GetArray();

            int inputType = inputArray[0].GetInt();

            if (inputType >= 4 && inputType <= 5) {
                std::string v = inputArray[1].GetString();

                newBlock->inputs[inputName] = std::stof(v);
            } else if (inputType >= 6 && inputType <= 8) {
                std::string v = inputArray[1].GetString();

                newBlock->inputs[inputName] = std::stoi(v);
            } else if (inputType >= 9 && inputType <= 10) {
                std::string v = inputArray[1].GetString();
                newBlock->inputs[inputName] = v;
            } else {
                throw std::runtime_error("Invalid input type!");
            }
        }
                
        auto& nextJson = v["next"];
        if (!nextJson.IsNull()) {
            newBlock->next = parseBlock(blockRoot, blockRoot[nextJson.GetString()]);
        }

        return newBlock;
    };

    std::vector<std::shared_ptr<ScratchBlock>> blocks;
    for (std::string& key : topLevelKeys) {
        blocks.push_back(parseBlock(blocksNode, blocksNode[key.c_str()]));
    }

    return std::move(blocks);
}

static std::vector<std::shared_ptr<ScratchTarget>> parseTargetsFromJSONNode(rapidjson::Value& targetsJson) {
    std::vector<std::shared_ptr<ScratchTarget>> targets;
    for (auto& targetJson : targetsJson.GetArray()) {
        auto target = std::make_shared<ScratchTarget>();
        target->isStage = targetJson["isStage"].GetBool();
        target->name = targetJson["name"].GetString();

        // Parse the costumes
        target->costumes = parseCostumesFromJSONNode(targetJson["costumes"]);

        // Initialize all the variables
        auto& varsJson = targetJson["variables"];
        for (auto it = varsJson.MemberBegin(); it != varsJson.MemberEnd(); it++) {
            std::string varId = it->name.GetString();

            auto& val = it->value.GetArray()[1];
            auto type = val.GetType();

            if (type == rapidjson::Type::kNumberType) {
                target->variables[varId] = val.GetFloat();
            } else if (type == rapidjson::Type::kStringType) {
                target->variables[varId] = std::string(val.GetString());
            } else {
                assert("aaaaa");
            }
        }

        target->blocks = parseBlocksFromJSONNode(targetJson["blocks"]);

        target->currentCostume = targetJson["currentCostume"].GetInt();
        target->posX = targetJson.HasMember("x") ? targetJson["x"].GetFloat() : 0;
        target->posY = targetJson.HasMember("y") ? targetJson["y"].GetFloat() : 0;
        target->size = targetJson.HasMember("size") ? targetJson["size"].GetFloat() : 100;
        target->direction = targetJson.HasMember("direction") ? targetJson["direction"].GetFloat() : 90;
        target->draggable = targetJson.HasMember("draggable") ? targetJson["draggable"].GetBool() : false;
        target->visible = targetJson.HasMember("visible") ? targetJson["visible"].GetBool() : true;
        target->layerOrder = targetJson["layerOrder"].GetInt();
        
        targets.push_back(target);
    }

    return std::move(targets);
}

std::shared_ptr<ScratchProject> ParseProjectJSON(std::string filepath) {
    std::ifstream file(filepath);
    std::stringstream ss;
    ss << file.rdbuf();
    std::string fileString = ss.str();

    rapidjson::Document document;
    document.Parse(fileString.c_str());

    auto project = std::make_shared<ScratchProject>();
    project->targets = parseTargetsFromJSONNode(document["targets"]);

    return project;
}