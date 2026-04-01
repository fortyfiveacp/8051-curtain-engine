#pragma once
#include "Component.h"
#include "Entity.h"
#include "Vector2D.h"

class ItemFactory {
    static void createBaseItem(Entity& entity, Vector2D position, float textureX, float textureY);
    static void createPointItem(Entity& entity, Vector2D position);
    static void createLargePowerItem(Entity& entity, Vector2D position);
    static void createSmallPowerItem(Entity& entity, Vector2D position);
    static void createBombItem(Entity& entity, Vector2D position);
public:
    static void createItem(Entity& entity, ItemType type, Vector2D position);
};
