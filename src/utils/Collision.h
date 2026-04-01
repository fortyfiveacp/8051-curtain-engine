#pragma once
#include "Component.h"
#include "SDL3/SDL_rect.h"

class Collision {
public:
    // Axis-aligned bounding box.
    static bool AABB(const SDL_FRect& rectA, const SDL_FRect& rectB);
    static bool AABB(const RectCollider& colA, const RectCollider& colB);
};
