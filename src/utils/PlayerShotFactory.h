#pragma once
#include "Component.h"
#include "Entity.h"

class World;

class PlayerShotFactory {
public:
    static void buildPlayerDanmaku(Entity& player, World& world, const PlayerShot& playerShot);
};