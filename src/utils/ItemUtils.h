#pragma once
#include "Component.h"

class ItemUtils {
public:
    static void enableItemHoming(Entity& itemEntity, Entity& targetEntity) {
        if (itemEntity.hasComponent<LookAtRotator>() && itemEntity.hasComponent<Velocity>() &&
            itemEntity.hasComponent<ItemBounce>() && targetEntity.hasComponent<Transform>()) {
            auto& itemRotator = itemEntity.getComponent<LookAtRotator>();
            auto& itemVelocity = itemEntity.getComponent<Velocity>();
            auto& itemBounce = itemEntity.getComponent<ItemBounce>();

            // Disable item bounce when homing.
            itemBounce.isBouncing = false;

            // Set homing target.
            itemRotator.target = &targetEntity.getComponent<Transform>();
            itemRotator.enabled = true;

            // Change velocity to local space, ensure correct direction and increase speed.
            itemVelocity.isLocalSpace = true;
            itemVelocity.direction = Vector2D(0, 1);
            itemVelocity.baseSpeed = 550.0f;
            itemVelocity.currentSpeed = 550.0f;
        }
    }
};
