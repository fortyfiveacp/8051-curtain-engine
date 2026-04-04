#include "EnemyFactory.h"

#include "DanmakuFactory.h"
#include "manager/AssetManager.h"

class World;

void EnemyFactory::buildEnemy(Entity &entity, World& world, const Convoy& convoyData) {
    auto& transform = entity.addComponent<Transform>(Vector2D(0.0f, -50.0f), 0.0f, 1.0f);
    entity.addComponent<PathFollower>(convoyData.pathId, 0.0f, convoyData.speed);
    entity.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f);
    entity.addComponent<EnemyTag>();

    switch (convoyData.enemyType) {
        case EnemyType::SmallFairy:
            buildSmallFairy(entity, transform, world);
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

void EnemyFactory::buildSmallFairy(Entity &entity, Transform &transform, World& world) {
    initBaseFairy(entity, "redFairy", "../asset/animations/small_fairies_anim.png", 32);

    // TODO: implement linear danmaku pattern that targets the player
}

void EnemyFactory::buildLargeFairy(Entity &entity, World& world, const DanmakuPattern& danmakuPattern) {
    initBaseFairy(entity, "redFairy", "../asset/animations/small_fairies_anim.png", 64);
    DanmakuFactory::buildDanmaku(entity, world, danmakuPattern);
}

void EnemyFactory::buildStageBoss(Entity &entity, Transform &transform) {

}

void EnemyFactory::initBaseFairy(Entity &entity, const std::string& animName, const char* spritesheetPath, float size) {
    entity.addComponent<Animation>(AssetManager::getAnimation(animName));

    SDL_Texture* tex = TextureManager::load(spritesheetPath);
    SDL_FRect src {0, 0, size, size};
    SDL_FRect dst {0, 0, size, size};

    entity.addComponent<Sprite>(tex, src, dst);

    auto& col = entity.addComponent<Collider>("projectile");
    col.rect.w = dst.w / 2.0f;
    col.rect.h = dst.w / 2.0f;
    col.offset.x = (dst.w - col.rect.w) / 2.0f;
    col.offset.y = (dst.h - col.rect.h) / 2.0f;
}