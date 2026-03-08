#pragma once
#include <string>

#include "Component.h"

class AssetManager {
    static std::unordered_map<std::string, Animation> animations;
    static Animation loadAnimationFromXML(const char* path);

public:
    static void loadAnimation(const std::string& clipname, const char* path);
    static const Animation& getAnimation(const std::string& clipName);
};
