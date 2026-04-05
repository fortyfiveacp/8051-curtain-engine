#pragma once
#include "Component.h"
#include "SDL3/SDL_rect.h"

class Collision {
public:
    // Axis-aligned bounding box.
    static bool AABB(const SDL_FRect& rectA, const SDL_FRect& rectB);
    static bool AABB(const RectCollider& colA, const RectCollider& colB);

    // Circle collision.
    static bool Circle(const CircleCollider& colA, const CircleCollider& colB);

    // Circle x AABB collision.
    static bool CircleAABB(const SDL_FRect& colA, const Vector2D& colBCenterPosition, const float& colBRadius);
    static bool CircleAABB(const RectCollider& colA, const CircleCollider& colB);
};
