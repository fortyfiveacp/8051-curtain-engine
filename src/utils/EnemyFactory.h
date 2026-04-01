#pragma once
#include "Component.h"
#include "manager/AssetManager.h"
#include "manager/TextureManager.h"

class EnemyFactory {
public:
    static void build(Entity& entity, EnemyType type, int pathId, float speed);

private:
    static void buildSmallFairy(Entity& entity, Transform& transform);
    static void buildLargeFairy(Entity& entity, Transform& transform);
    static void buildStageBoss(Entity& entity, Transform& transform);
};
