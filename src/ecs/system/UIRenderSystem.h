#pragma once
#include <memory>
#include <vector>

#include "Entity.h"

class UIRenderSystem {
public:
    void render(const std::vector<std::unique_ptr<Entity>>& entities);
};
