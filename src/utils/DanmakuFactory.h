#pragma once
#include "Component.h"
#include "Entity.h"

class World;

class DanmakuFactory {
public:
    static void buildDanmaku(Entity& entity, World& world, const DanmakuPattern& danmakuPattern);
private:
    static void buildRadialDanmaku(Entity &entity, World &world, const DanmakuPattern &danmakuPattern);
    static void buildLinearDanmaku(Entity &entity, World &world, const DanmakuPattern &danmakuPattern);

    static void initRadialPattern(Entity& entity, World& world, const DanmakuPattern &danmakuPattern);
    static void initLinearPattern(Entity& entity, World& world, const DanmakuPattern &danmakuPattern);
};
