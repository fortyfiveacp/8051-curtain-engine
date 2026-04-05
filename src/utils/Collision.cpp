#include "Collision.h"

// Axis-aligned bounding box
// Passing in rects
bool Collision::AABB(const SDL_FRect& rectA, const SDL_FRect& rectB) {
    // Is the right edge of rectA >= left edge of rectB
    // Is the right edge of rectB >= left edge of rectA
    // Is the bottom edge of rectA >= top edge of rectB
    // Is the bottom edge of rectB >= top edge of rectA

    if (rectA.x + rectA.w >= rectB.x &&
        rectB.x + rectB.w >= rectA.x &&
        rectA.y + rectA.h >= rectB.y &&
        rectB.y + rectB.h >= rectA.y) {
        return true;
    }

    return false;
}

// Passing in the colliders itself
bool Collision::AABB(const RectCollider& colA, const RectCollider& colB) {
    if (AABB(colA.rect, colB.rect)) {
        return true;
    }

    return false;
}

// Circle collision.
bool Collision::Circle(const CircleCollider& colA, const CircleCollider& colB) {
    auto& colAPosition = colA.centerPosition;
    auto& colBPosition = colB.centerPosition;
    float distanceBetweenColliders = (colBPosition - colAPosition).length();

    return distanceBetweenColliders <= colA.radius + colB.radius;
}

// AABB x Circle collision.
bool Collision::AABBCircle(const SDL_FRect& colA, const Vector2D& colBCenterPosition, const float& colBRadius) {
    // Convert colB position and radius into an FRect square approximation.
    const SDL_FRect& colB = SDL_FRect(
        colBCenterPosition.x - colBRadius, colBCenterPosition.y - colBRadius, colBRadius * 2, colBRadius * 2);

    return AABB(colA, colB);
}

// AABB x Circle collision.
bool Collision::AABBCircle(const RectCollider& colA, const CircleCollider& colB) {
    return AABBCircle(colA.rect, colB.centerPosition, colB.radius);
}
