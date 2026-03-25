#include "EnemyFactory.h"

void EnemyFactory::build(Entity &entity, EnemyType type, Vector2D pos) {
    auto& transform = entity.addComponent<Transform>(pos, 0.0f, 1.0f);

    switch (type) {
        case EnemyType::SmallFairy:
            // TODO: find spritesheet of small fairy - use bird spritesheet as placeholder for now.
            buildSmallFairy(entity, transform);
            break;
        case EnemyType::LargeFairy:
            // TODO: find spritesheet of large fairy
            break;
        case EnemyType::Boss:
            // TODO: find spritesheet of boss
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

    entity.addComponent<Sprite>(tex, src, dst);
    entity.addComponent<Collider>("enemy", dst);
    entity.addComponent<Health>(10);
}