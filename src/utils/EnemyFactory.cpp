#include "EnemyFactory.h"

class World;

void EnemyFactory::build(Entity &entity, EnemyType type, int pathId, float speed, World& world) {
    auto& transform = entity.addComponent<Transform>(Vector2D(0.0f, 0.0f), 0.0f, 1.0f);
    entity.addComponent<PathFollower>(pathId, 0.0f, speed);
    entity.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f);
    entity.addComponent<ProjectileTag>();

    switch (type) {
        case EnemyType::SmallFairy:
            // TODO: find spritesheet of small fairy - use bird spritesheet as placeholder for now.
            buildSmallFairy(entity, transform);
            break;
        case EnemyType::LargeFairy:
            // TODO: find spritesheet of large fairy - use spritesheet as placeholder for now.
            buildLargeFairy(entity, transform, world);
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

    auto& col = entity.addComponent<Collider>("projectile");
    col.rect.w = dst.w / 2.0f;
    col.rect.h = dst.h / 2.0f;

    col.offset.x = (dst.w - col.rect.w) / 2.0f;
    col.offset.y = (dst.h - col.rect.h) / 2.0f;

    entity.addComponent<Sprite>(tex, src, dst);
}

void EnemyFactory::buildLargeFairy(Entity &entity, Transform &transform, World &world) {
    entity.addComponent<Animation>(AssetManager::getAnimation("enemy"));
    SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
    SDL_FRect src {0, 0, 64, 64};
    SDL_FRect dst {transform.position.x, transform.position.y, 64, 64};

    auto& col = entity.addComponent<Collider>("projectile");
    col.rect.w = dst.w / 2.0f;
    col.rect.h = dst.h / 2.0f;

    col.offset.x = (dst.w - col.rect.w) / 2.0f;
    col.offset.y = (dst.h - col.rect.h) / 2.0f;

    entity.addComponent<Sprite>(tex, src, dst);

    bool currentGlobalFiringState = false;
    for (auto& e : world.getEntities()) {
        if (e->hasComponent<StageState>()) {
            currentGlobalFiringState = e->getComponent<StageState>().largeFairiesShouldFire;
            break;
        }
    }

    float frequency = 2.0f;
    float bulletSpeed = 100.0f;
    float bulletAngularVel = 0.0f;
    float radius = 30.0f;
    int bulletsPerBurst = 8;

    float rotSpeed = 50.0f;

    entity.addComponent<RadialSpawner>(
        currentGlobalFiringState,
        rotSpeed,           // rotationSpeed
        frequency,          // frequency
        bulletSpeed,        // bulletEmissionSpeed
        bulletAngularVel,   // bulletEmissionAngularVelocity
        radius,             // radius
        bulletsPerBurst,    // bulletsPerBurst
        [&world, &entity, bulletSpeed, bulletAngularVel, radius](Vector2D direction) {
            if (!entity.isActive()) {
                return;
            }

            auto& fairyTransform = entity.getComponent<Transform>();
            auto& bullet = world.createDeferredEntity();

            Vector2D spawnPos = fairyTransform.position + (direction * radius);
            bullet.addComponent<Transform>(spawnPos, 0.0f, 1.0f);
            bullet.addComponent<Velocity>(direction, bulletSpeed, true);
            bullet.addComponent<AngularVelocity>(bulletAngularVel);
            bullet.addComponent<ProjectileTag>();

            SDL_Texture* tex = TextureManager::load("../asset/bullet4.png");
            SDL_FRect src {0, 0, 64, 64};
            SDL_FRect dst {fairyTransform.position.x, fairyTransform.position.y, 64, 64};
            bullet.addComponent<Sprite>(tex, src, dst);

            auto& bulletCol = bullet.addComponent<Collider>("projectile");
            bulletCol.rect.w = dst.w / 2.5f;
            bulletCol.rect.h = dst.h / 2.5f;

            bulletCol.offset.x = (dst.w - bulletCol.rect.w) / 2.0f;
            bulletCol.offset.y = (dst.h - bulletCol.rect.h) / 2.0f;
        }
    );
}

void EnemyFactory::buildStageBoss(Entity &entity, Transform &transform) {

}
