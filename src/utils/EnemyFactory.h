#pragma once
#include "Component.h"
#include "World.h"

class EnemyFactory {
public:
    static void buildEnemy(Entity &entity, World &world, const Convoy &convoyData);

private:
    static void buildSmallFairy(Entity &entity, World &world, const EnemyType &enemyType, const DanmakuPattern &danmakuPattern);
    static void buildLargeFairy(Entity &entity, World &world, const DanmakuPattern &danmakuPattern);
    static void initBaseFairy(Entity& entity, const std::string& animName, const char* spritesheetPath, float size);

};
