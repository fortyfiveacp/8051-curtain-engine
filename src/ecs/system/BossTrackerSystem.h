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

                // If no boss, disable the tracker.
                if (bossEntity == nullptr) {
                    trackerSprite.visible = false;
                    return;
                }

                // Enable sprite visibility when boss is active.
                trackerSprite.visible = true;

                auto& trackerTransform = entity->getComponent<Transform>();
                auto& bossTransform = bossEntity->getComponent<Transform>();

                // Sync tracker with boss, clamping in bounds.
                trackerTransform.position.x = std::clamp(bossTransform.position.x, tracker.minX, tracker.maxX);
            }
        }
    }
};
