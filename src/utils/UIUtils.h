#pragma once
#include "Component.h"
#include "Entity.h"

class UiUtils {
public:
    static void updateIconLabel(Entity& entity) {
        auto& iconLabel = entity.getComponent<IconLabel>();

        if (!iconLabel.dirty) {
            return;
        }

        if (entity.hasComponent<Children>()) {
            auto& children = entity.getComponent<Children>();

            for (auto& child : children.children) {
                child->getComponent<Sprite>().visible = false;
            }

            for (int i = 0; i < iconLabel.currentNumber; i++) {
                children.children[i]->getComponent<Sprite>().visible = true;
            }
        }
    }
};
