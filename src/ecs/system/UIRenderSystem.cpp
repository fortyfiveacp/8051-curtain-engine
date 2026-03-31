#include "UIRenderSystem.h"
#include "Component.h"
#include "RenderUtils.h"
#include "manager/TextureManager.h"

void UIRenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities) {
    for (auto& entity : entities) {
        if (entity->hasComponent<Transform>()) {
            auto transform = entity->getComponent<Transform>();

            if (entity->hasComponent<Sprite>()) {
                auto sprite = entity->getComponent<Sprite>();

                // Only render sprites on the UI layer.
                if (sprite.renderLayer != RenderLayer::UI) {
                    continue;
                }

                // No converting from world space to screen space for UI.
                sprite.dst.x = transform.position.x;
                sprite.dst.y = transform.position.y;

                if (sprite.visible) {
                    SDL_FRect scaledDest = RenderUtils::getScaledDest(sprite.dst, transform.scale);
                    TextureManager::draw(sprite.texture, &sprite.src, &scaledDest);
                }
            } else if (entity->hasComponent<Label>()) {
                auto label = entity->getComponent<Label>();

                label.dst.x = transform.position.x;
                label.dst.y = transform.position.y;

                if (label.visible) {
                    SDL_FRect scaledDest = RenderUtils::getScaledDest(label.dst, transform.scale);
                    TextureManager::draw(label.texture, nullptr, &scaledDest);
                }
            }
        }
    }
}