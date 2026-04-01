#pragma once
#include <memory>
#include <unordered_map>
#include "Entity.h"
#include "Component.h"

class World;

class PathSystem {
public:
    float getPathLength(const Path& path);
    Vector2D evaluatePath(const Path& path, float distance);

    void update(World& world, std::vector<std::unique_ptr<Entity>>& entities, float dt);
};
