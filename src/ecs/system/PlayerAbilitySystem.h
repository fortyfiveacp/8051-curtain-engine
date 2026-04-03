#pragma once
#include <memory>
#include <vector>

#include "Entity.h"

class PlayerAbilitySystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities);
};
