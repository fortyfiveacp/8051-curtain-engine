#pragma once
#include <vector>

#include "Component.h"
#include "Entity.h"

class DestructionSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* cameraEntity = nullptr;

        // Find camera
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) {
            return;
        }

        auto& cam = cameraEntity->getComponent<Camera>();

        for (auto& e : entities) {
            if (e->hasComponent<Transform>() && (e->hasComponent<ProjectileTag>() || e->hasComponent<EnemyTag>() || e->hasComponent<PlayerShot>())) {
                auto& t = e->getComponent<Transform>();

                // Destroy entity if it goes out the cam view
                if (t.position.x > cam.view.x + cam.view.w + cam.outOfViewPadding || // Right edge of view
                    t.position.x < cam.view.x - cam.outOfViewPadding|| // Left edge of view
                    t.position.y > cam.view.y + cam.view.h + cam.outOfViewPadding|| // Bottom edge of view
                    t.position.y < cam.view.y - cam.outOfViewPadding) // Top edge of view
                {
                    e->destroy();
                }
            }
        }
    }
};
