#pragma once
#include "Component.h"
#include "Entity.h"

class World;

class BossFactory {
public:
    static void buildStageBoss(Entity &entity, World &world, const Boss &bossData, const Vector2D &startPos);
};
