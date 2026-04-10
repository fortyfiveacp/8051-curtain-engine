#include "DanmakuFactory.h"
#include "World.h"

void DanmakuFactory::initRadialPattern(Entity& entity, World& world, const DanmakuPattern& danmakuPattern) {
    entity.addComponent<RadialSpawner>(
        true,
        danmakuPattern.rotationSpeed,
        danmakuPattern.frequency,
        danmakuPattern.bulletSpeed,
        danmakuPattern.bulletAngularVel,
        danmakuPattern.radius,
        danmakuPattern.bulletsPerBurst,
        [&world, danmakuPattern, &entity](Vector2D direction) {
            if (!entity.isActive()) {
                return;
            }

            auto& transform = entity.getComponent<Transform>();
            Vector2D spawnPos = transform.position + (direction * danmakuPattern.radius);

            auto& bullet = world.createDeferredEntity();

            auto& bulletTransform = bullet.addComponent<Transform>(spawnPos, 0.0f, 1.0f);
            bullet.addComponent<Velocity>(direction, danmakuPattern.bulletSpeed, true);
            bullet.addComponent<AngularVelocity>(danmakuPattern.bulletAngularVel);
            bullet.addComponent<ProjectileTag>();

            // TODO: If multiple bullet types are implemented, should probably move this to a helper method.
            SDL_Texture* tex = TextureManager::load("../asset/bullet4.png");
            SDL_FRect src {192, 0, 64, 64};
            SDL_FRect dst {spawnPos.x, spawnPos.y, 64, 64};
            Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
            bullet.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

            Vector2D bulletSpawnPositionOffset = direction * danmakuPattern.radius;
            auto& bulletCol = bullet.addComponent<CircleCollider>("projectile");
            bulletCol.centerPosition = bulletTransform.position + bulletSpawnPositionOffset;
            bulletCol.radius = danmakuPattern.radius / 1.2;

            AudioManager::playSfx("circle-bullet-shot");
        }
    );
}

void DanmakuFactory::initLinearPattern(Entity &entity, World &world, const DanmakuPattern &danmakuPattern) {
    entity.addComponent<LinearSpawner>(
        true,
        danmakuPattern.isFanPattern,
        danmakuPattern.bulletSpeed,
        danmakuPattern.speedMultiplier,
        danmakuPattern.bulletPositions,
        danmakuPattern.frequency,
        [&world, &entity, danmakuPattern](Vector2D position, Vector2D direction, float speed) {
            if (!entity.isActive()) {
                return;
            }
            auto& bullet = world.createDeferredEntity();
            bullet.addComponent<Transform>(position, 0.0f, 1.0f);
            bullet.addComponent<Velocity>(direction, speed, false);
            bullet.addComponent<ProjectileTag>();

            // TODO: If multiple bullet types are implemented, should probably move this to a helper method.
            SDL_Texture* tex = TextureManager::load("../asset/bullet4.png");
            SDL_FRect src {192, 0, 64, 64};
            SDL_FRect dst {position.x, position.y, 32, 32};
            Vector2D pivotOffset = Vector2D(dst.w / 2.0f, dst.h / 2.0f);
            bullet.addComponent<Sprite>(tex, src, dst, RenderLayer::World, pivotOffset);

            auto& bulletCol = bullet.addComponent<CircleCollider>("projectile");
            bulletCol.centerPosition = position;
            bulletCol.radius = 4;

            AudioManager::playSfx("circle-bullet-shot", 0.5f);
        }
    );
}

void DanmakuFactory::buildRadialDanmaku(Entity& entity, World& world, const DanmakuPattern& danmakuPattern) {
    auto& timeline = entity.hasComponent<Timeline>() ? entity.getComponent<Timeline>() : entity.addComponent<Timeline>();

    timeline.timeline.emplace_back(danmakuPattern.startTime, [&entity, &world, danmakuPattern]() {
        if (entity.isActive()) {
            initRadialPattern(entity, world, danmakuPattern);
        }
    });

    timeline.timeline.emplace_back(danmakuPattern.endTime, [&entity]() {
        if (entity.isActive() && entity.hasComponent<RadialSpawner>()) {
            entity.getComponent<RadialSpawner>().isActive = false;
        }
    });
}

void DanmakuFactory::buildLinearDanmaku(Entity& entity, World& world, const DanmakuPattern& danmakuPattern) {
    auto& timeline = entity.hasComponent<Timeline>() ? entity.getComponent<Timeline>() : entity.addComponent<Timeline>();

    timeline.timeline.emplace_back(danmakuPattern.startTime, [&entity, &world, danmakuPattern]() {
        if (entity.isActive()) {
            initLinearPattern(entity, world, danmakuPattern);
        }
    });

    timeline.timeline.emplace_back(danmakuPattern.endTime, [&entity]() {
        if (entity.isActive() && entity.hasComponent<LinearSpawner>()) {
            entity.getComponent<LinearSpawner>().isActive = false;
        }
    });
}

void DanmakuFactory::buildDanmaku(Entity &entity, World &world, const DanmakuPattern &danmakuPattern) {
    if (danmakuPattern.hasPattern) {
        switch (danmakuPattern.danmakuType) {
            case DanmakuType::Linear:
                buildLinearDanmaku(entity, world, danmakuPattern);
                break;
            case DanmakuType::Radial:
                buildRadialDanmaku(entity, world, danmakuPattern);
                break;
        }
    }
}
