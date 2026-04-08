#pragma once

#include "Component.h"

class ComponentUtils {
public:
    static void resetTimeline(Timeline& timeline) {
        timeline.currentTime = 0.0f;
        timeline.timeline.clear();
    }

    static void deactivateSpawners(Entity* entity) {
        if (entity->hasComponent<RadialSpawner>()) {
            entity->getComponent<RadialSpawner>().isActive = false;
        }
        if (entity->hasComponent<LinearSpawner>()) {
            entity->getComponent<LinearSpawner>().isActive = false;
        }
    }
};