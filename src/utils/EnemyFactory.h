#pragma once
#include "Component.h"
#include "manager/AssetManager.h"
#include "manager/TextureManager.h"

class EnemyFactory {
public:
    static void build(Entity& entity, EnemyType type, Vector2D pos);

private:
    static void buildSmallFairy(Entity& entity, Transform& transform);

};
