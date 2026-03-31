#pragma once
#include "Component.h"
#include "Entity.h"
#include "Vector2D.h"

class ItemFactory {
    static void createBaseItem(Entity& entity, Transform transform, float textureX, float textureY);
    static void createPointItem(Entity& entity, Transform transform);
    static void createSmallPowerItem(Entity& entity, Transform transform);
    static void createLargePowerItem(Entity& entity, Transform transform);
    static void createBombItem(Entity& entity, Transform transform);
public:
    static void createItem(Entity& entity, ItemType type, Vector2D position);
};
