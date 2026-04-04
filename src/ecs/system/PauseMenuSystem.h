#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class World;

class PauseMenuSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event);
};
