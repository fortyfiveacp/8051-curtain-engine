#pragma once
#include "Component.h"

class World;

class ConvoySystem {
public:
    void update(World& world, float dt);

private:
    void spawnEnemy(World& world, const Convoy& convoy);
};
