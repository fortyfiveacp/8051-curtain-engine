#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class RadialSpawnerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<RadialSpawner>() && entity->hasComponent<Transform>()) {
                auto& spawner = entity->getComponent<RadialSpawner>();
                auto& transform = entity->getComponent<Transform>();
                spawner.spawnTimer -= dt;

                if (spawner.spawnTimer <= 0) {
                    spawner.spawnTimer += spawner.frequency;

                    float angleBetweenBullets = 360.0f / spawner.bulletsPerBurst;

                    float currentEmissionAngle = transform.rotation;
                    // std::cout << "Spawner Emission Angle: " << currentEmissionAngle << std::endl;

                    for (int i = 0; i < spawner.bulletsPerBurst; i++) {
                        spawner.spawnCallback(
                            Vector2D(currentEmissionAngle));

                        currentEmissionAngle += angleBetweenBullets;
                    }
                }
            }
        }
    }
};
