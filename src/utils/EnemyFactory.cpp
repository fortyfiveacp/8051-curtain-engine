#include "EnemyFactory.h"

#include "DanmakuFactory.h"
#include "ItemFactory.h"
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
            buildLargeFairy(entity, world, convoyData.danmakuPattern);
            break;
        default:
            break;
    }
}

void EnemyFactory::buildSmallFairy(Entity &entity, World& world, const EnemyType& enemyType, const DanmakuPattern& danmakuPattern) {
    entity.addComponent<EnemyHealth>(100, 100);
    auto& loot = entity.addComponent<LootDropTable>();
    loot.offsets = { {0, 0} };

    if (enemyType == EnemyType::SmallRedFairy) {
        initBaseFairy(entity, "redFairy", "../asset/animations/small_fairies_anim.png", 32);
        loot.types = { SmallPower };
    } else {
        initBaseFairy(entity, "blueFairy", "../asset/animations/small_fairies_anim.png", 32);
        loot.types = { Point };
    }
    initLinearLookAt(entity, danmakuPattern, world.getEntities());
    DanmakuFactory::buildDanmaku(entity, world, danmakuPattern);
}

void EnemyFactory::buildLargeFairy(Entity &entity, World& world, const DanmakuPattern& danmakuPattern) {
    initBaseFairy(entity, "largeFairy", "../asset/animations/large_fairy_anim.png", 64);

    entity.addComponent<EnemyHealth>(500);

    auto& loot = entity.addComponent<LootDropTable>();
    loot.types = { Point, Point, LargePower, Point, Point };
    loot.offsets = { {-50, 0}, {50, 0}, {0, 0}, {0, -50}, {0, 50} };

    initLinearLookAt(entity, danmakuPattern, world.getEntities());
    DanmakuFactory::buildDanmaku(entity, world, danmakuPattern);
}

void EnemyFactory::initBaseFairy(Entity &entity, const std::string& animName, const char* spritesheetPath, float size) {
    auto& transform = entity.addComponent<Transform>(Vector2D(0.0f, -50.0f), 0.0f, 1.0f);
    entity.addComponent<Animation>(AssetManager::getAnimation(animName));

    SDL_Texture* tex = TextureManager::load(spritesheetPath);
    SDL_FRect src {0, 0, size, size};
    SDL_FRect dst {0, 0, size, size};

    Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
    entity.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

    auto& collider = entity.addComponent<CircleCollider>("enemy");
    collider.centerPosition = transform.position;
    collider.radius = size/ 4;
}

// Adds a LookAtRotator to the fairy if they use a Linear Danmaku Pattern.
void EnemyFactory::initLinearLookAt(Entity& entity, const DanmakuPattern& danmakuPattern, std::vector<std::unique_ptr<Entity>>& entities) {
    if (danmakuPattern.shouldTargetPlayer) {
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                auto& playerTransform = e->getComponent<Transform>();
                entity.addComponent<LookAtRotator>(&playerTransform, 0.0f);
                break;
            }
        }
    }
}