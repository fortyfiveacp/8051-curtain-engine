#include "PathSystem.h"
#include "World.h"

float PathSystem::getPathLength(const Path& path)  {
    float totalLength = 0.0f;
    for (size_t i = 0; i < path.points.size() - 1; i++) {
        totalLength += (path.points[i + 1].position - path.points[i].position).length();
    }
    return totalLength;
}

Vector2D PathSystem::evaluatePath(const Path& path, float distance) {
    float remainingPoints = distance;

    for (size_t i = 0; i < path.points.size() - 1; i++) {
        auto a = path.points[i].position;
        auto b = path.points[i + 1].position;

        float length = (b - a).length();

        if (remainingPoints <= length) {
            float t = remainingPoints / length;
            return a + (b - a) * t;
        }

        remainingPoints -= length;
    }

    return path.points.back().position;
}

void PathSystem::update(World& world, std::vector<std::unique_ptr<Entity>>& entities, float dt) {
    for (auto& e : entities) {
        if (!e->hasComponent<PathFollower>() || !e->hasComponent<Transform>() || !e->hasComponent<Velocity>())
            continue;

        auto& pf = e->getComponent<PathFollower>();
        auto& tf = e->getComponent<Transform>();
        auto& vel = e->getComponent<Velocity>();
        if (!pf.active) continue;

        // const Path& path = (*pathLibrary)[pf.pathId];
        const Path& path = world.getPathLibrary()[pf.pathId];
        if (pf.currentHoverTimer > 0) {
            pf.currentHoverTimer -= dt;
            continue;
        }

        // Determine if there's an upcoming hover point.
        float nextDistance = pf.distance + (pf.speed * dt);
        float accumulatedDist = 0.0f;

        bool isHovering = false;

        for (int i = 0; i < static_cast<int>(path.points.size()) - 1; ++i) {
            float segmentLen = (path.points[i + 1].position - path.points[i].position).length();
            accumulatedDist += segmentLen;

            // Check if next move is past next point, if it has a wait time, and if enemy has not hovered
            // there yet.
            if (pf.distance < accumulatedDist && nextDistance >= accumulatedDist) {
                if (path.points[i + 1].hoverTime > 0 && pf.lastPointReached < i + 1) {
                    pf.currentHoverTimer = path.points[i + 1].hoverTime;
                    pf.lastPointReached = i + 1;
                    pf.distance = accumulatedDist;
                    tf.position = path.points[i + 1].position;

                    isHovering = true;
                    break;
                }
            }
        }

        if (!isHovering) {
            pf.distance = nextDistance;
            Vector2D nextPosition = evaluatePath(path, nextDistance);

            Vector2D direction = nextPosition - tf.position;
            vel.direction = direction.normalize();

            tf.position = nextPosition;

        }
    }
}
