#pragma once
#include "Component.h"
#include "World.h"
#include "manager/AssetManager.h"
#include "manager/TextureManager.h"

class EnemyFactory {
public:
    static void build(Entity& entity, EnemyType type, int pathId, float speed, World& world);

private:
    static void buildSmallFairy(Entity& entity, Transform& transform, World& world);
    static void buildLargeFairy(Entity& entity, Transform& transform, World& world);
    static void buildStageBoss(Entity& entity, Transform& transform);
};
