#pragma once
#include <memory>
#include <bits/stl_vector.h>

#include "Entity.h"

class World;

class DeathBombSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>> &entities, float deltaTime);
};
