#pragma once
#include "Component.h"
#include "Entity.h"

class World;

class BossFactory {
public:
    static void buildStageBoss(Entity &entity, World &world, const Boss &bossData, const Vector2D &startPos, const std::vector<Vector2D> &emitterOffsets);
private:
    static void createChildEmitter(Entity &parent, World &world, Vector2D offset, const DanmakuPattern &pattern);
};
