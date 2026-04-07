#pragma once
#include "Component.h"
#include "World.h"
#include "manager/AssetManager.h"

class BossFactory {
public:
    static void buildStageBoss(Entity& entity, World& world, const Boss& bossData) {
        // 1. Setup Main Boss Entity
        entity.addComponent<Boss>(bossData); // Copy the parsed data over
        entity.addComponent<EnemyHealth>(bossData.maxHealth, bossData.maxHealth);
        entity.addComponent<EnemyTag>();
        entity.addComponent<Velocity>(); // Required for collision/movement logic

        // Spawn off-screen top center
        auto& transform = entity.addComponent<Transform>(Vector2D(384.0f, -100.0f), 0.0f, 1.0f);
        auto& col = entity.addComponent<CircleCollider>("enemy");
        col.centerPosition = transform.position;
        col.radius = 16.0f;

        SDL_Texture* tex = TextureManager::load("../asset/animations/sanae_anim.png");
        SDL_FRect src {0, 0, 64, 64};
        SDL_FRect dst {0, 0, 64, 64};
        Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
        entity.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);
        entity.addComponent<Animation>(AssetManager::getAnimation("sanae"));

        auto& childrenComp = entity.addComponent<Children>();

        // 2. Setup Invisible Children
        auto& bossRef = entity.getComponent<Boss>();
        for (const auto& offset : bossRef.childOffsets) {
            auto& childEntity = world.createDeferredEntity();

            // Transform is absolute, so we offset from parent's initial position
            childEntity.addComponent<Transform>(Vector2D(384.0f + offset.x, -100.0f + offset.y), 0.0f, 1.0f);
            childEntity.addComponent<Parent>(&entity);

            // No Sprite component means it remains invisible!

            bossRef.childrenEntities.push_back(&childEntity);
            childrenComp.children.push_back(&childEntity);
        }
    }
};
