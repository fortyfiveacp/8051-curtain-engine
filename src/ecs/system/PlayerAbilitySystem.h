#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class World;

class PlayerAbilitySystem {
public:
    void update(World& world, float deltaTime);

private:
    void castBomb(Entity &entity, const Transform &transform, World &world);
};
