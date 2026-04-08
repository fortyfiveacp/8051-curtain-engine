#include "BossFactory.h"
#include "TransformUtils.h"
#include "World.h"
#include "manager/AssetManager.h"

void BossFactory::buildStageBoss(Entity &entity, World &world, const Boss &bossData, const Vector2D& startPos) {
    auto& boss = entity.addComponent<Boss>();
    boss.maxHealth = 1000;
    boss.currentHealth = bossData.maxHealth;
    boss.bossName = "Sanae Kochiya";
    boss.phasesLeft = 2;

    std::cout << boss.bossName << std::endl;
    std::cout << boss.maxHealth << std::endl;
    std::cout << boss.currentHealth << std::endl;
    std::cout << boss.phasesLeft << std::endl;

    auto& transform = entity.addComponent<Transform>();
    transform.position = startPos;

    entity.addComponent<Animation>(AssetManager::getAnimation("sanae"));

    SDL_Texture* tex = TextureManager::load("../asset/animations/sanae_anim.png");
    SDL_FRect src {0, 0, 63, 65};
    SDL_FRect dst {0, 0, 63, 65};

    Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
    entity.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

    auto& collider = entity.addComponent<CircleCollider>("enemy");
    collider.centerPosition = transform.position;
    collider.radius = 16;

    entity.addComponent<EnemyTag>();

    auto& children = entity.addComponent<Children>();

    auto& leftEmitter = world.createDeferredEntity();
    leftEmitter.addComponent<Transform>().position = startPos + Vector2D(-60, 20);
    leftEmitter.addComponent<Parent>().parent = &entity;
    leftEmitter.addComponent<RadialSpawner>();
    children.children.push_back(&leftEmitter);

    auto& rightEmitter = world.createDeferredEntity();
    rightEmitter.addComponent<Transform>().position = startPos + Vector2D(60, 20);
    rightEmitter.addComponent<Parent>().parent = &entity;
    rightEmitter.addComponent<RadialSpawner>();
    children.children.push_back(&rightEmitter);
}
