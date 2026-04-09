#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"
#include "ItemUtils.h"

class ItemHomingSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* itemHomingTargetEntity = nullptr;

        // Find item homing target.
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerTag>() && entity->hasComponent<Transform>() &&
                entity->hasComponent<PlayerBombAbility>()) {
                itemHomingTargetEntity = entity.get();
            }
        }

        // If no target, return.
        if (itemHomingTargetEntity == nullptr) {
            return;
        }
        auto& playerBombAbility = itemHomingTargetEntity->getComponent<PlayerBombAbility>();

        // Make all items home to target if a bomb is active.
        if (playerBombAbility.active) {
            for (auto& entity : entities) {
                if (entity->hasComponent<Item>() && entity->hasComponent<Velocity>() && entity->hasComponent<LookAtRotator>()) {
                    ItemUtils::enableItemHoming(*entity, *itemHomingTargetEntity);
                }
            }
        }
    }
};
