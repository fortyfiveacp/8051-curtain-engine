#include "DebugRenderSystem.h"

#include "World.h"
#include "event/BaseEvent.h"

void DebugRenderSystem::update(World& world, const SDL_Event& event, bool isDebugging) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) {
        // Enable debugging via event.
        world.getEventManager().emit(DebugEvent{!isDebugging});
    }
}

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities) {
    for (auto& entity : entities) {
        if (entity->hasComponent<Collider>()) {
            auto& c = entity->getComponent<Collider>();

            SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
            SDL_Texture* circleTex = TextureManager::load("../asset/ball.png");

            SDL_FRect colSrc {32, 32, 32, 32};
            SDL_FRect circleSrc {0, 0, 32, 32};

            if (c.shape == ColliderShape::Box) {
                TextureManager::draw(tex, &colSrc, &c.rect);
            }
            else if (c.shape == ColliderShape::Circle) {
                SDL_FRect destRect {
                    c.rect.x - c.radius,
                    c.rect.y - c.radius,
                    c.radius * 2.0f,
                    c.radius * 2.0f
                };

                TextureManager::draw(circleTex, &circleSrc, &destRect);
            }
        }
    }
}

void DrawCircle(SDL_Renderer* renderer, float centerX, float centerY, float radius) {
    const float diameter = (radius * 2);

    float x = (radius - 1);
    float y = 0;
    float tx = 1;
    float ty = 1;
    float error = (tx - diameter);

    while (x >= y) {
        // Each of these draws a point in one of the 8 octants of the circle
        SDL_RenderPoint(renderer, centerX + x, centerY - y);
        SDL_RenderPoint(renderer, centerX + x, centerY + y);
        SDL_RenderPoint(renderer, centerX - x, centerY - y);
        SDL_RenderPoint(renderer, centerX - x, centerY + y);
        SDL_RenderPoint(renderer, centerX + y, centerY - x);
        SDL_RenderPoint(renderer, centerX + y, centerY + x);
        SDL_RenderPoint(renderer, centerX - y, centerY - x);
        SDL_RenderPoint(renderer, centerX - y, centerY + x);

        if (error <= 0) {
            y++;
            error += ty;
            ty += 2;
        }
        if (error > 0) {
            x--;
            tx += 2;
            error += (tx - diameter);
        }
    }
}