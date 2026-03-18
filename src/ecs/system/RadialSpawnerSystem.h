#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class RadialSpawnerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<RadialSpawner>()) {
                auto& spawner = entity->getComponent<RadialSpawner>();
                spawner.spawnTimer -= dt;

                if (spawner.spawnTimer <= 0) {
                    std::cout << "Spawn" << std::endl;
                    spawner.spawnTimer += spawner.frequency;

                    float angleBetweenBullets = 360.0f / spawner.bulletsPerBurst;

                    // TODO: have a way to change the initial emission angle in the future?
                    float currentEmissionAngle = 0.0f;

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
