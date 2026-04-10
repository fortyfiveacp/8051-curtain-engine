#pragma once
#include "Component.h"
#include "Entity.h"
#include "Vector2D.h"
#include "World.h"

// Normally, in Touhou games, you can choose between different characters. Each character usually has
// a different kind of bomb, so implementing said bombs can be done here. At the current state of the game,
// only one kind of bomb is needed.
class PlayerBombFactory {
public:
    static void buildBasicBomb(World& world, Vector2D position, const float size) {
        auto& bomb = world.createDeferredEntity();
        bomb.addComponent<PlayerBomb>();
        bomb.addComponent<Transform>(position, 0.0f, 1.0f);
        bomb.addComponent<Velocity>(Vector2D(0, -1), 100.0f);

        SDL_Texture* tex = TextureManager::load("../asset/objects/bomb.png");
        SDL_FRect src = {0, 0, 64, 64};
        SDL_FRect dst = {0, 0, size, size};
        bomb.addComponent<Sprite>(tex, src, dst, RenderLayer::WorldBackground);

        auto& col = bomb.addComponent<CircleCollider>("bomb");
        float radius = size / 2.0f;
        col.radius = radius;
        col.offset = { radius, radius };

        auto& fadeIn = bomb.addComponent<Fade>();
        fadeIn.isFading = true;
        fadeIn.startingAlpha = 0;
        fadeIn.endingAlpha = 100;
        fadeIn.fadeDuration = 0.5f;
    }
};
