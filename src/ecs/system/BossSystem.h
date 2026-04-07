#pragma once
#include "Component.h"
#include "TransformUtils.h"
#include "DanmakuFactory.h"

class World;

class BossSystem {
public:
    void update(World& world, float dt);

private:
    void triggerInvulnerability(Boss& boss);

    void applyPattern(World& world, Entity* bossEntity, Boss& boss, bool isThreshold);

    void clearSpawners(Entity* e);

    void die(Entity* bossEntity, Boss& boss);
};
