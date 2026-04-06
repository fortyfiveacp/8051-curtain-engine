#pragma once
#include <algorithm>
#include <memory>

#include "Component.h"

class BossHealthBarSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        Entity* bossEntity = nullptr;

        for (auto& entity : entities) {
            if (entity->hasComponent<Boss>()) {
                bossEntity = entity.get();
            }
        }

        if (!bossEntity) {
            return;
        }

        auto& boss = bossEntity->getComponent<Boss>();

        // TODO: temporarily get player stats for testing.
        Entity* playerEntity = nullptr;
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerStats>()) {
                playerEntity = entity.get();
            }
        }

        for (auto& entity : entities) {
            if (entity->hasComponent<BossHealthBar>() && entity->hasComponent<Sprite>() &&
                entity->hasComponent<Children>() && entity->hasComponent<Fade>()) {
                auto& bossHealthBar = entity->getComponent<BossHealthBar>();
                auto& sprite = entity->getComponent<Sprite>();
                auto& children = entity->getComponent<Children>();
                auto& fade = entity->getComponent<Fade>();

                auto& playerStats = playerEntity->getComponent<PlayerStats>(); // TODO: temp.

                // If the health bar is not initialized (the boss has just spawned), initialize it.
                if (!bossHealthBar.isInitialized) {
                    bossHealthBar.timer += dt;

                    // Extend the bar to full.
                    sprite.dst.w = bossHealthBar.fullDstWidth * (bossHealthBar.timer / bossHealthBar.initializationDuration);

                    // Finish initialization.
                    if (bossHealthBar.timer >= bossHealthBar.initializationDuration) {
                        bossHealthBar.isInitialized = true;
                    }
                } else {
                    // float healthPercentage = std::clamp(
                    //     (float)boss.currentHealth / boss.maxHealth, 0.0f, 1.0f
                    // );

                    // TODO: temporarily hooked up to player stats for testing.
                    float healthPercentage = std::clamp(
                        (float)playerStats.currentHealth / 3, 0.0f, 1.0f
                    );

                    // Crop health bar destination to visibly lower the health bar.
                    sprite.dst.w = bossHealthBar.fullDstWidth * healthPercentage;
                }

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
                        // TODO: temporarilty hooked up to player stats for testing.
                        counter.currentNumber = playerStats.currentHealth;
                        counter.dirty = true;
                    }
                }
            }
        }
    }
};
