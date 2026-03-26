#pragma once
#include <memory>
#include <unordered_map>
#include "Entity.h"
#include "Component.h"

class PathSystem {
    std::unordered_map<int, Path>* pathLibrary;

public:
    PathSystem(std::unordered_map<int, Path>* lib) : pathLibrary(lib) {}

    float getPathLength(const Path& path) {
        float totalLength = 0.0f;
        for (size_t i = 0; i < path.points.size() - 1; i++) {
            totalLength += (path.points[i + 1] - path.points[i]).getLength();
        }
        return totalLength;
    }

    Vector2D evaluatePath(const Path& path, float distance) {
        float remainingPoints = distance;

        for (size_t i = 0; i < path.points.size() - 1; i++) {
            auto a = path.points[i];
            auto b = path.points[i + 1];

            float length = (b - a).getLength();

            if (remainingPoints <= length) {
                float t = remainingPoints / length;
                return a + (b - a) * t;
            }

            remainingPoints -= length;
        }

        return path.points.back();
    }

    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (!e->hasComponent<PathFollower>() || !e->hasComponent<Transform>())
                continue;

            auto& pf = e->getComponent<PathFollower>();
            auto& tf = e->getComponent<Transform>();

            if (!pf.active) continue;

            const Path& path = (*pathLibrary)[pf.pathId];

            if (pf.distance >= getPathLength(path)) {
                e->destroy();
                continue;
            }

            tf.oldPosition = tf.position;
            pf.distance += pf.speed * dt;
            tf.position = evaluatePath(path, pf.distance);
        }
    }
};