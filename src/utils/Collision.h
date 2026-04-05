#pragma once
#include "Component.h"
#include "SDL3/SDL_rect.h"

class Collision {
public:
    // Axis-aligned bounding box.
    static bool AABB(const SDL_FRect& rectA, const SDL_FRect& rectB);
    static bool AABB(const Collider& colA, const Collider& colB);

    static bool CircleCircle(Vector2D posA, float radA, Vector2D posB, float radB);
    static bool CircleRect(Vector2D circleCenter, float radius, const SDL_FRect& rect);
};
