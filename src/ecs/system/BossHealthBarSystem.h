#pragma once
#include <algorithm>
#include <memory>

#include "Component.h"

class BossHealthBarSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        Entity *bossEntity = nullptr;

        // Find the boss.
        for (auto &entity: entities) {
            if (entity->hasComponent<Boss>()) {
                bossEntity = entity.get();
            }
        }

        // TODO: temporarily get player stats for testing.
        // Entity *playerEntity = nullptr;
        // for (auto &entity: entities) {
        //     if (entity->hasComponent<PlayerStats>()) {
        //         playerEntity = entity.get();
        //     }
        // }

        for (auto &entity: entities) {
            if (entity->hasComponent<BossHealthBar>() && entity->hasComponent<Sprite>() &&
                entity->hasComponent<Children>() && entity->hasComponent<Fade>() && entity->hasComponent<Toggleable>()) {
                auto &bossHealthBar = entity->getComponent<BossHealthBar>();
                auto &sprite = entity->getComponent<Sprite>();
                auto &children = entity->getComponent<Children>();
                auto &fade = entity->getComponent<Fade>();

                if (bossEntity == nullptr) {
                    // If the boss health bar is initialized but no boss was found, it must have spawned and died,
                    // so toggle the health bar off.
                    if (bossHealthBar.isInitialized) {
                        entity->getComponent<Toggleable>().toggle();
                        bossHealthBar.isInitialized = false;
                        bossHealthBar.timer = 0.0f;
                    }

                    return;
                }

                auto &boss = bossEntity->getComponent<Boss>();
                // auto &playerStats = playerEntity->getComponent<PlayerStats>(); // TODO: temp.

                // If the health bar is not initialized (the boss has just spawned), initialize it first.
                if (!bossHealthBar.isInitialized) {
                    bossHealthBar.timer += dt;

                    // Extend the bar to full.
                    sprite.dst.w = bossHealthBar.fullDstWidth * (bossHealthBar.timer / bossHealthBar.initializationDuration);

                    // Finish initialization.
                    if (bossHealthBar.timer >= bossHealthBar.initializationDuration) {
                        bossHealthBar.isInitialized = true;
                    }

                    return;
                }
                // float healthPercentage = std::clamp(
                //     static_cast<float>(boss.currentHealth) / boss.maxHealth, 0.0f, 1.0f
                // );
                // TODO: temporarily hooked up to player stats for testing.
                float healthPercentage = std::clamp(
                    static_cast<float>(boss.currentHealth) / boss.maxHealth, 0.0f, 1.0f
                );

                // Crop health bar destination to visibly lower the health bar.
                sprite.dst.w = bossHealthBar.fullDstWidth * healthPercentage;

                // Update children.
                for (auto &child : children.children) {
                    if (child->hasComponent<Label>()) {
                        auto &label = child->getComponent<Label>();
                        label.text = boss.bossName;
                        label.dirty = true;
                    }

                    if (child->hasComponent<IconCounter>()) {
                        auto &counter = child->getComponent<IconCounter>();
                        counter.currentNumber = boss.phasesLeft;
                        // TODO: temporarily hooked up to player stats for testing.
                        // counter.currentNumber = playerStats.currentHealth;
                        counter.dirty = true;
                    }
                }
            }
        }
    }
};
