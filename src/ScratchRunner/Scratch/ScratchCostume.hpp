#pragma once

#include <string>
#include <vector>
#include <cstdint>

class ScratchCostume {
public:
    std::string name;
    int bitmapRes;

    int rotationCenterX;
    int rotationCenterY;

    std::vector<std::uint8_t> pixels;
    int pixelWidth;
    int pixelHeight;
};