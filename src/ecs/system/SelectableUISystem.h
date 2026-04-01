#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"

class World;

class SelectableUISystem {
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities, World& world, const SDL_Event &event);
};
