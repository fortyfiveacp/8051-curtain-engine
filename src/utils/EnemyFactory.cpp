#include "EnemyFactory.h"

class World;

void EnemyFactory::build(Entity &entity, EnemyType type, int pathId, float speed, World& world) {
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

    Collider col = entity.addComponent<Collider>("projectile");
    col.rect.w = dst.w;
    col.rect.h = dst.h;
    entity.addComponent<Collider>(col);

    entity.addComponent<Sprite>(tex, src, dst);
    entity.addComponent<Health>(10);
}

void EnemyFactory::buildLargeFairy(Entity &entity, Transform &transform, World &world) {
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

    float frequency = 1.0f;
    float bulletSpeed = 150.0f;
    float bulletAngularVel = 20.0f;
    float radius = 30.0f;
    int bulletsPerBurst = 31;

    entity.addComponent<RadialSpawner>(
        0.0f,                   // rotationSpeed
        frequency,          // frequency
        bulletSpeed,        // bulletEmissionSpeed
        bulletAngularVel,   // bulletEmissionAngularVelocity
        radius,             // radius
        10.0f,                  // duration
        2.0f,                   // delay
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
            bullet.addComponent<Sprite>(tex, SDL_FRect{0,0,64,64}, SDL_FRect{0,0,64,64});

            auto& c = bullet.addComponent<Collider>("projectile");
            c.rect.w = 32;
            c.rect.h = 32;
        }
    );
}

void EnemyFactory::buildStageBoss(Entity &entity, Transform &transform) {

}
