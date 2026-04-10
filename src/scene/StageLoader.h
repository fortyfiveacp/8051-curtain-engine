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

    static DanmakuPattern parseDanmakuPattern(tinyxml2::XMLElement *elem);

    static void loadPaths(tinyxml2::XMLElement* pathsRoot, World& world);
    static void loadWaves(tinyxml2::XMLElement* wavesRoot, World& world, Timeline& timelineComponent);
    static void loadBoss(tinyxml2::XMLElement* bossElem, World& world, Timeline& timelineComponent);

    static void initBossIntro(World& world);
};
