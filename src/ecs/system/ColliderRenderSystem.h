#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "../../manager/TextureManager.h"

class World;

class ColliderRenderSystem {
public:
    void update(World& world, const SDL_Event& event, bool isDebugging);
    void render(const std::vector<std::unique_ptr<Entity>>& entities, bool isDebugging);
};
