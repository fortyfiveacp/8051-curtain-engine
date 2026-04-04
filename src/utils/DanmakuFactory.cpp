#include "DanmakuFactory.h"
#include "World.h"

void DanmakuFactory::initRadialPattern(Entity& entity, World& world, const RadialConfig& config) {
    Entity* entityPtr = &entity;

    entity.addComponent<RadialSpawner>(
        true,
        config.rotationSpeed,
        config.frequency,
        config.bulletSpeed,
        config.bulletAngularVel,
        config.radius,
        config.bulletsPerBurst,
        [&world, config, entityPtr](Vector2D direction) {
            if (!entityPtr) {
                return;
            }

            auto& tf = entityPtr->getComponent<Transform>();
            Vector2D spawnPos = tf.position + (direction * config.radius);

            auto& bullet = world.createDeferredEntity();

            bullet.addComponent<Transform>(spawnPos, 0.0f, 1.0f);
            bullet.addComponent<Velocity>(direction, config.bulletSpeed, true);
            bullet.addComponent<AngularVelocity>(config.bulletAngularVel);
            bullet.addComponent<ProjectileTag>();

            // TODO: If multiple bullet types are implemented, should probably move this to a helper method.
            SDL_Texture* tex = TextureManager::load("../asset/bullet4.png");
            SDL_FRect src {192, 0, 64, 64};
            SDL_FRect dst {spawnPos.x, spawnPos.y, 64, 64};
            bullet.addComponent<Sprite>(tex, src, dst);

            auto& bulletCol = bullet.addComponent<Collider>("projectile");
            bulletCol.rect.w = dst.w / 2.5f;
            bulletCol.rect.h = dst.h / 2.5f;

            bulletCol.offset.x = (dst.w - bulletCol.rect.w) / 2.0f;
            bulletCol.offset.y = (dst.h - bulletCol.rect.h) / 2.0f;
        }
    );
}

void DanmakuFactory::buildRadialDanmaku(Entity& entity, World& world, const DanmakuPattern& danmakuPattern, const RadialConfig& config) {
    auto& timeline = entity.hasComponent<Timeline>() ? entity.getComponent<Timeline>() : entity.addComponent<Timeline>();
    Entity* entityPtr = &entity;

    timeline.timeline.emplace_back(danmakuPattern.startTime, [entityPtr, &world, config]() {
        if (entityPtr && entityPtr->isActive()) {
            initRadialPattern(*entityPtr, world, config);
        }
    });

    timeline.timeline.emplace_back(danmakuPattern.endTime, [entityPtr]() {
        if (entityPtr && entityPtr->hasComponent<RadialSpawner>()) {
            entityPtr->getComponent<RadialSpawner>().isActive = false;
        }
    });
}

void DanmakuFactory::buildDanmaku(Entity &entity, World &world, const DanmakuPattern &danmakuPattern) {
    if (danmakuPattern.hasPattern) {
        switch (danmakuPattern.danmakuType) {
            case DanmakuType::Linear:
                break;
            case DanmakuType::Radial:
                RadialConfig config{};
                config.frequency = danmakuPattern.frequency;
                config.bulletSpeed = danmakuPattern.bulletSpeed;
                config.bulletsPerBurst = danmakuPattern.bulletsPerBurst;

                buildRadialDanmaku(entity, world, danmakuPattern, config);
        }
    }
}
