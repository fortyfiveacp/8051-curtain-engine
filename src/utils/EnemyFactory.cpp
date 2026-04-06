#include "EnemyFactory.h"

#include "DanmakuFactory.h"
#include "manager/AssetManager.h"

class World;

void EnemyFactory::buildEnemy(Entity &entity, World& world, const Convoy& convoyData) {
    entity.addComponent<PathFollower>(convoyData.pathId, 0.0f, convoyData.speed);
    entity.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f);
    entity.addComponent<EnemyTag>();

    switch (convoyData.enemyType) {
        case EnemyType::SmallBlueFairy:
        case EnemyType::SmallRedFairy:
            buildSmallFairy(entity, world, convoyData.enemyType, convoyData.danmakuPattern);
            break;
        case EnemyType::LargeFairy:
            // TODO: find spritesheet of large fairy - use small fairy spritesheet as placeholder.
            buildLargeFairy(entity, world, convoyData.danmakuPattern);
            break;
        case EnemyType::Boss:
            // TODO: find spritesheet of boss.
            break;
        default:
            break;
    }
}

void EnemyFactory::buildSmallFairy(Entity &entity, World& world, const EnemyType& enemyType, const DanmakuPattern& danmakuPattern) {
    if (enemyType == EnemyType::SmallRedFairy) {
        initBaseFairy(entity, "redFairy", "../asset/animations/small_fairies_anim.png", 32);
    } else {
        initBaseFairy(entity, "blueFairy", "../asset/animations/small_fairies_anim.png", 32);
    }

    if (danmakuPattern.shouldTargetPlayer) {
        for (auto& e : world.getEntities()) {
            if (e->hasComponent<PlayerTag>()) {
                auto& playerTransform = e->getComponent<Transform>();
                entity.addComponent<LookAtRotator>(playerTransform, 0.0f);
                break;
            }
        }
    }

    DanmakuFactory::buildDanmaku(entity, world, danmakuPattern);
}

void EnemyFactory::buildLargeFairy(Entity &entity, World& world, const DanmakuPattern& danmakuPattern) {
    initBaseFairy(entity, "largeFairy", "../asset/animations/large_fairy_anim.png", 64);
    DanmakuFactory::buildDanmaku(entity, world, danmakuPattern);
}

void EnemyFactory::buildStageBoss(Entity &entity, Transform &transform) {

}

void EnemyFactory::initBaseFairy(Entity &entity, const std::string& animName, const char* spritesheetPath, float size) {
    auto& transform = entity.addComponent<Transform>(Vector2D(0.0f, -50.0f), 0.0f, 1.0f);
    entity.addComponent<Animation>(AssetManager::getAnimation(animName));

    SDL_Texture* tex = TextureManager::load(spritesheetPath);
    SDL_FRect src {0, 0, size, size};
    SDL_FRect dst {0, 0, size, size};

    Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
    entity.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

    float radius = size / 2.0f;
    Vector2D bulletSpawnPositionOffset = transform.position * radius;
    auto& bulletCol = entity.addComponent<CircleCollider>("enemy");
    bulletCol.centerPosition = transform.position + bulletSpawnPositionOffset;
    bulletCol.radius = radius / 2;
}