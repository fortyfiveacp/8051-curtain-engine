#include "EnemyFactory.h"

void EnemyFactory::build(Entity &entity, EnemyType type, int pathId, float speed) {
    auto& transform = entity.addComponent<Transform>(Vector2D(0.0f, 0.0f), 0.0f, 1.0f);
    entity.addComponent<PathFollower>(pathId, 0.0f, speed);
    entity.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f);

    switch (type) {
        case EnemyType::SmallFairy:
            // TODO: find spritesheet of small fairy - use bird spritesheet as placeholder for now.
            buildSmallFairy(entity, transform);
            break;
        case EnemyType::LargeFairy:
            // TODO: find spritesheet of large fairy - use spritesheet as placeholder for now.
            buildLargeFairy(entity, transform);
            break;
        case EnemyType::Boss:
            // TODO: find spritesheet of boss.
            break;
        default:
            break;
    }
}

void EnemyFactory::buildSmallFairy(Entity &entity, Transform &transform) {
    entity.addComponent<Animation>(AssetManager::getAnimation("enemy"));
    SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
    SDL_FRect src {0, 0, 32, 32};
    SDL_FRect dst {transform.position.x, transform.position.y, 32, 32};

    Collider col = entity.addComponent<Collider>("projectile");
    col.rect.w = dst.w;
    col.rect.h = dst.h;
    entity.addComponent<Collider>(col);

    entity.addComponent<Sprite>(tex, src, dst);
    entity.addComponent<Health>(10);
}

void EnemyFactory::buildLargeFairy(Entity &entity, Transform &transform) {
    entity.addComponent<Animation>(AssetManager::getAnimation("enemy"));
    SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
    SDL_FRect src {0, 0, 64, 64};
    SDL_FRect dst {transform.position.x, transform.position.y, 64, 64};

    Collider col = entity.addComponent<Collider>("projectile");
    col.rect.w = dst.w;
    col.rect.h = dst.h;
    entity.addComponent<Collider>(col);

    entity.addComponent<Sprite>(tex, src, dst);
    entity.addComponent<Health>(10);
}

void EnemyFactory::buildStageBoss(Entity &entity, Transform &transform) {

}
