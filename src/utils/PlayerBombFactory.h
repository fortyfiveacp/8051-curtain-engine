#pragma once
#include "Component.h"
#include "Entity.h"
#include "Vector2D.h"
#include "World.h"

class PlayerBombFactory {
public:
    static void buildBasicBomb(World& world, Vector2D position, const float size) {
        auto& bomb = world.createDeferredEntity();
        bomb.addComponent<PlayerBomb>();
        bomb.addComponent<Transform>(position, 0.0f, 1.0f);
        bomb.addComponent<Velocity>(Vector2D(0, -1), 60.0f);

        SDL_Texture* tex = TextureManager::load("../asset/coin.png");
        SDL_FRect src = {0, 0, 32, 32};
        SDL_FRect dst = {0, 0, size, size};
        bomb.addComponent<Sprite>(tex, src, dst, RenderLayer::World);

        auto& col = bomb.addComponent<Collider>("bomb");
        col.shape = ColliderShape::Circle;
        float radius = size / 2.0f;
        col.radius = radius;
        col.offset = { radius, radius };
    }
};
