#pragma once
#include <memory>

#include "Component.h"
#include "Entity.h"
#include "ItemUtils.h"

class ItemHomingSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* playerEntity = nullptr;

        // Find player.
        for (auto& entity : entities) {
            if (entity->hasComponent<PlayerTag>() && entity->hasComponent<Transform>() &&
                entity->hasComponent<PlayerBombAbility>()) {
                playerEntity = entity.get();
            }
        }

        // If no player, return.
        if (playerEntity == nullptr) {
            return;
        }
        auto& playerBombAbility = playerEntity->getComponent<PlayerBombAbility>();
        auto& playerTransform = playerEntity->getComponent<Transform>();

        // The distance from the top of the stage that the player needs to be above to auto collect items.
        float itemCollectionZoneThreshold = 294.0f;

        // Make all items home to target if a bomb is active or player is above the item collection zone threshold.
        if (playerBombAbility.active || playerTransform.position.y < itemCollectionZoneThreshold) {
            for (auto& entity : entities) {
                if (entity->hasComponent<Item>() && entity->hasComponent<Velocity>() && entity->hasComponent<LookAtRotator>()) {
                    ItemUtils::enableItemHoming(*entity, *playerEntity);
                }
            }
        }
    }
};
