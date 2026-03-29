#pragma once
#include "Component.h"
#include "Entity.h"

class UiUtils {
public:
    static void updateIconCounter(Entity& entity) {
        auto& iconCounter = entity.getComponent<IconCounter>();

        if (!iconCounter.dirty) {
            return;
        }

        // The actual icons should be children of the entity.
        if (entity.hasComponent<Children>()) {
            auto& children = entity.getComponent<Children>();

            // Check if the current number is greater than the capacity of the counter.
            // Logic for ensuring the current number stays within the bound should be handled elsewhere.
            if (iconCounter.currentNumber > children.children.size()) {
                std::cerr << "Icon counter has exceeded the set maximum size!" << std::endl;
            }

            // Make the current number of icons visible, then make the rest invisible.
            for (int i = 0; i < children.children.size(); i++) {
                if (i < iconCounter.currentNumber) {
                    children.children[i]->getComponent<Sprite>().visible = true;
                } else {
                    children.children[i]->getComponent<Sprite>().visible = false;
                }
            }
        }
    }
};
