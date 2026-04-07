#pragma once
#include <string>

#include "Component.h"
#include "tinyxml2.h"
#include "World.h"

class StageLoader {
public:
    static void loadStage(const char* path, World& world);
private:
    static EnemyType stringToEnemyType(const std::string& name);
    static DanmakuType stringToDanmakuType(const std::string& name);
    static BulletType stringToBulletType(const std::string& name);
    static DanmakuPattern readDanmakuPattern(tinyxml2::XMLElement* patternElement);
};
