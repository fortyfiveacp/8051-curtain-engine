#pragma once
#include "Component.h"
#include "Entity.h"

class World;

class PlayerShotFactory {
public:
    static void buildPlayerDanmaku(Entity& player, World& world, const Vector2D& playerPosition, PlayerStats& playerStats);
private:
    static float powerToDamage(float baseDamage, int power);
};