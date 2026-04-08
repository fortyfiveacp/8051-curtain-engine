#include "BossFactory.h"
#include "TransformUtils.h"
#include "World.h"
#include "manager/AssetManager.h"

void BossFactory::buildStageBoss(Entity &entity, World &world, const Boss &bossData, const Vector2D& startPos, const std::vector<Vector2D>& emitterOffsets) {
    auto& transform = entity.addComponent<Transform>();
    transform.position = startPos;

    entity.addComponent<Animation>(AssetManager::getAnimation("sanae"));
    entity.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f);

    SDL_Texture* tex = TextureManager::load("../asset/animations/sanae_anim.png");
    SDL_FRect src {0, 0, 63, 65};
    SDL_FRect dst {0, 0, 63, 65};

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
}

void BossFactory::createChildEmitter(Entity &parent, World &world, Vector2D offset) {
    auto& emitter = world.createDeferredEntity();
    auto& pTransform = parent.getComponent<Transform>();
    auto& childrenComp = parent.getComponent<Children>();

    emitter.addComponent<Transform>().position = pTransform.position + offset;
    emitter.addComponent<Parent>().parent = &parent;
    // emitter.addComponent<RadialSpawner>(); // TODO: read from the XML for danmaku configurations

    // TODO: remove once danmaku can be successfully spawned from emitters.
    SDL_Texture* tex = TextureManager::load("../asset/animations/sanae_anim.png");
    SDL_FRect src {0, 0, 63, 65};
    SDL_FRect dst {0, 0, 63, 65};
    Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
    emitter.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

    childrenComp.children.push_back(&emitter);
}
