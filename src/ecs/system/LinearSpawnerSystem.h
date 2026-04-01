#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class LinearSpawnerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<LinearSpawner>() && entity->hasComponent<Transform>()) {
                auto& spawner = entity->getComponent<LinearSpawner>();
                auto& transform = entity->getComponent<Transform>();
                spawner.lifetime += dt;

                // We might end up using Timeline for this depending on how complicated our danmaku gets.
                // For now, we will just be using this simple setup with delay and duration.
                if (spawner.lifetime < spawner.delay || spawner.lifetime > spawner.delay + spawner.duration) {
                    return;
                }

                spawner.spawnTimer -= dt;

                if (spawner.spawnTimer <= 0) {
                    spawner.spawnTimer += spawner.frequency;

                    for (int i = 0; i < spawner.bulletPositions.size(); i++) {
                        auto& localPosition = spawner.bulletPositions[i];

                        // Rotate the local position according to the spawner's rotation.
                        float angleRadians = std::atan2(localPosition.y, localPosition.x) +
                            transform.rotation * (std::numbers::pi / 180.0f);
                        float radius = localPosition.length();

                        auto rotatedLocalPosition = Vector2D(std::cos(angleRadians) * radius, std::sin(angleRadians) * radius);
                        const Vector2D spawnPosition = transform.position + rotatedLocalPosition;
                        Vector2D displacement = -rotatedLocalPosition;

                        float rotation;

                        if (spawner.isFanPattern) {
                            rotation = std::atan2(displacement.y, displacement.x) + std::numbers::pi / 2;
                        }
                        else {
                            rotation = 0;
                        }

                        const float spawnSpeed = spawner.bulletEmissionSpeed +
                            displacement.length() * spawner.bulletEmissionSpeedMultiplier;

                        const auto direction = Vector2D(rotation * (180 / std::numbers::pi));

                        spawner.spawnCallback(spawnPosition, direction, spawnSpeed);
                    }
                }
            }
        }
    }
};
