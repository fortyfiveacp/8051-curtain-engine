#pragma once
#include <memory>
#include <vector>

#include "Entity.h"

class World;

class PlayerAbilitySystem {
public:
    void update(World& world, float deltaTime);
};
