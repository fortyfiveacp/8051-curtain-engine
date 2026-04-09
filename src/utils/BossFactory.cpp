#include "BossFactory.h"

#include "DanmakuFactory.h"
#include "World.h"
#include "manager/AssetManager.h"

void BossFactory::buildStageBoss(Entity &entity, World &world, const Boss &bossData, const Vector2D& startPos, const std::vector<Vector2D>& emitterOffsets) {
    auto& transform = entity.addComponent<Transform>();
    transform.position = startPos;

    entity.addComponent<Animation>(AssetManager::getAnimation("sanae"));
    entity.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f);

    SDL_Texture* tex = TextureManager::load("../asset/animations/sanae_anim.png");
    SDL_FRect src {0, 0, 92, 125};
    SDL_FRect dst {0, 0, 73.6, 100}; // Scale down the destination rect a bit.

    Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
    entity.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

    auto& collider = entity.addComponent<CircleCollider>("boss");
    collider.centerPosition = transform.position;
    collider.radius = 16;

    entity.addComponent<EnemyTag>();
    entity.addComponent<Children>();
    entity.addComponent<Boss>(bossData);

    for (const auto& offset : emitterOffsets) {
        createChildEmitter(entity, world, offset);
    }

    if (!bossData.phaseList.empty()) {
        const auto& firstPhase = bossData.phaseList[0];

        if (!firstPhase.patterns.empty()) {
            if (firstPhase.target == PatternTarget::Boss) {
                DanmakuFactory::buildDanmaku(entity, world, firstPhase.patterns[0]);
            } else {
                auto& children = entity.getComponent<Children>();
                for (size_t i = 0; i < children.children.size(); ++i) {
                    Entity* child = children.children[i];
                    if (!child) {
                        continue;
                    }

                    const auto& pattern = firstPhase.patterns[i % firstPhase.patterns.size()];
                    DanmakuFactory::buildDanmaku(*child, world, pattern);
                }
            }
        }
    }
}

void BossFactory::createChildEmitter(Entity &parent, World &world, Vector2D offset) {
    auto& emitter = world.createDeferredEntity();
    auto& pTransform = parent.getComponent<Transform>();
    auto& childrenComp = parent.getComponent<Children>();

    emitter.addComponent<Transform>().position = pTransform.position + offset;
    emitter.addComponent<Parent>().parent = &parent;

    childrenComp.children.push_back(&emitter);
}