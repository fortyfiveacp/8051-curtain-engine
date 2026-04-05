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
bool Collision::AABB(const Collider& colA, const Collider& colB) {
    if (AABB(colA.rect, colB.rect)) {
        return true;
    }

    return false;
}

bool Collision::CircleCircle(Vector2D posA, float radA, Vector2D posB, float radB) {
    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float distanceSq = (dx * dx) + (dy * dy);
    float radiusSum = radA + radB;
    return distanceSq <= (radiusSum * radiusSum);
}

bool Collision::CircleRect(Vector2D circleCenter, float radius, const SDL_FRect& rect) {
    float closestX = std::max(rect.x, std::min(circleCenter.x, rect.x + rect.w));
    float closestY = std::max(rect.y, std::min(circleCenter.y, rect.y + rect.h));

    float dx = circleCenter.x - closestX;
    float dy = circleCenter.y - closestY;

    return (dx * dx + dy * dy) <= (radius * radius);
}