#pragma once
#include "Component.h"
#include "Entity.h"

class World;

class DanmakuFactory {
public:
    static void buildRadialDanmaku(Entity &entity, World &world, const DanmakuPattern &danmakuPattern, const RadialConfig &config);
    static void buildDanmaku(Entity& entity, World& world, const DanmakuPattern& danmakuPattern);

private:
    static void initRadialPattern(Entity& entity, World& world, const RadialConfig& config);
};
