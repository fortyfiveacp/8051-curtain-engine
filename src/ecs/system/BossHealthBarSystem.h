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

                // If the health bar is not initialized (the boss has just spawned), initialize it first.
                // But don't initialize if the boss is dead.
                if (!bossHealthBar.isInitialized && !bossEntity->hasComponent<DeadTag>()) {
                    bossHealthBar.timer += dt;

                    // Extend the bar to full.
                    sprite.dst.w = bossHealthBar.fullDstWidth * (bossHealthBar.timer / bossHealthBar.initializationDuration);

                    // Finish initialization.
                    if (bossHealthBar.timer >= bossHealthBar.initializationDuration) {
                        bossHealthBar.isInitialized = true;
                        bossHealthBar.timer = 0.0f;
                    }

                    return;
                }
                float healthPercentage = std::clamp(
                    static_cast<float>(boss.currentHealth) / boss.maxHealth, 0.0f, 1.0f
                );

                // Crop health bar destination to visibly lower the health bar.
                sprite.dst.w = bossHealthBar.fullDstWidth * healthPercentage;

                // If health hits 0, re-initialize.
                if (healthPercentage == 0.0f) {
                    bossHealthBar.isInitialized = false;
                }

                // Update children.
                for (auto &child : children.children) {
                    if (child->hasComponent<Label>()) {
                        auto &label = child->getComponent<Label>();

                        // If boss is not dead, ensure the label displays their name, if they are dead clear the label.
                        if (!bossEntity->hasComponent<DeadTag>()) {
                            label.text = boss.bossName;
                            label.dirty = true;
                        } else {
                            label.text = " ";
                            label.dirty = true;
                        }
                    }

                    if (child->hasComponent<IconCounter>()) {
                        auto &counter = child->getComponent<IconCounter>();
                        counter.currentNumber = boss.phasesLeft;
                        counter.dirty = true;
                    }
                }
            }
        }
    }
};
