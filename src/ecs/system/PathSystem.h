#pragma once
#include <memory>
#include <unordered_map>
#include "Entity.h"
#include "Component.h"

class PathSystem {
    std::unordered_map<int, Path>* pathLibrary;

public:
    PathSystem(std::unordered_map<int, Path>* lib)
        : pathLibrary(lib) {}

    float getPathLength(const Path& path) {
        float total = 0.0f;
        for (size_t i = 0; i < path.points.size() - 1; i++) {
            total += (path.points[i+1] - path.points[i]).getLength();
        }
        return total;
    }

    Vector2D evaluate(const Path& path, float distance) {
        float remaining = distance;

        for (size_t i = 0; i < path.points.size() - 1; i++) {
            auto a = path.points[i];
            auto b = path.points[i + 1];

            float len = (b - a).getLength();

            if (remaining <= len) {
                float t = remaining / len;
                return a + (b - a) * t;
            }

            remaining -= len;
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
            tf.position = evaluate(path, pf.distance);
        }
    }
};