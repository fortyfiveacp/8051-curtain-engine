#pragma once
#include <algorithm>
#include <memory>

#include "Component.h"

class BossHealthBarSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* bossEntity = nullptr;

        for (auto& entity : entities) {
            if (entity->hasComponent<Boss>()) {
                bossEntity = entity.get();
            }
        }

        if (!bossEntity) {
            return;
        }

        // TODO: temporarily get player stats for testing.
        Entity* playerEntity = nullptr;
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerStats>()) {
                playerEntity = entity.get();
            }
        }

        for (auto& entity : entities) {
            if (entity->hasComponent<BossHealthBar>() && entity->hasComponent<Sprite>() &&
                entity->hasComponent<Children>()) {
                auto& bossHealthBar = entity->getComponent<BossHealthBar>();
                auto& sprite = entity->getComponent<Sprite>();
                auto& boss = bossEntity->getComponent<Boss>();
                auto& children = entity->getComponent<Children>();

                // If the health bar is not initialized (the boss has just spawned), initialize it.
                if (!bossHealthBar.isInitialized) {
                    bossHealthBar.isInitialized = true;
                }

                // float healthPercentage = std::clamp(
                //     (float)boss.currentHealth / boss.maxHealth, 0.0f, 1.0f
                // );

                // TODO: temporarily hooked up to player for testing.
                auto& playerStats = playerEntity->getComponent<PlayerStats>();
                float healthPercentage = std::clamp(
                    (float)playerStats.currentHealth / 3, 0.0f, 1.0f
                );

                // Crop health bar destination to lower the bar.
                sprite.dst.w = bossHealthBar.fullDstWidth * healthPercentage;

                // Update children.
                for (auto& child : children.children) {
                    if (child->hasComponent<Label>()) {
                        auto& label = child->getComponent<Label>();
                        label.text = boss.bossName;
                        label.dirty = true;
                    }

                    if (child->hasComponent<IconCounter>()) {
                        auto& counter = child->getComponent<IconCounter>();
                        //counter.currentNumber = boss.phasesLeft;
                        // TODO: temporarilty hooked up to player for testing.
                        counter.currentNumber = playerStats.currentHealth;
                        counter.dirty = true;
                    }
                }
            }
        }
    }
};
