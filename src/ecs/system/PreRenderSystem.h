#pragma once
#include <memory>

#include "Component.h"
#include "manager/TextureManager.h"
#include "utils/UIUtils.h"

class PreRenderSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& e : entities) {
            if (e->hasComponent<Label>() && e->getComponent<Label>().dirty) {
                TextureManager::updateLabel(e->getComponent<Label>());
            } else if (e->hasComponent<IconCounter>()) {
                UiUtils::updateIconLabel(*e);
            }
        }
    }
};
