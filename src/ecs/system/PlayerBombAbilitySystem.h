#pragma once

#include "Component.h"
#include "Entity.h"

class World;

class PlayerBombAbilitySystem {
public:
    void update(World& world, float deltaTime);

private:
    void castBomb(Entity &entity, const Transform &transform, World &world);
};
