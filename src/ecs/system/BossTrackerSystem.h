#pragma once
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class BossTrackerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* bossEntity = nullptr;

        // Find boss.
        for (auto& entity : entities) {
            if (entity->hasComponent<Boss>() && entity->hasComponent<Transform>()) {
                bossEntity = entity.get();
            }
        }

        for (auto& entity : entities) {
            if (entity->hasComponent<BossTracker>() && entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                auto& tracker = entity->getComponent<BossTracker>();
                auto& trackerSprite = entity->getComponent<Sprite>();

                // If no boss, hide the tracker.
                if (bossEntity == nullptr) {
                    tracker.isInitialized = false;
                    trackerSprite.visible = false;
                    return;
                }

                // If a boss is active and the tracker hasn't initialized yet, perform initialization.
                if (!tracker.isInitialized) {
                    tracker.isInitialized = true;
                    trackerSprite.visible = true;

                    // Fade tracker in, if it has a fade component.
                    if (entity->hasComponent<Fade>()) {
                        entity->getComponent<Fade>().isFading = true;
                    }
                }

                auto& trackerTransform = entity->getComponent<Transform>();
                auto& bossTransform = bossEntity->getComponent<Transform>();


                // Sync tracker with boss on the X-axis, clamping in bounds.
                trackerTransform.position.x = std::clamp(bossTransform.position.x + trackerSprite.dst.w / 2,
                    tracker.minX, tracker.maxX - trackerSprite.dst.w);
            }
        }
    }
};
