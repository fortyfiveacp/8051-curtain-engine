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

            auto& tf = entity.getComponent<Transform>();
            Vector2D spawnPos = tf.position + (direction * danmakuPattern.radius);

            auto& bullet = world.createDeferredEntity();

            bullet.addComponent<Transform>(spawnPos, 0.0f, 1.0f);
            bullet.addComponent<Velocity>(direction, danmakuPattern.bulletSpeed, true);
            bullet.addComponent<AngularVelocity>(danmakuPattern.bulletAngularVel);
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

void DanmakuFactory::initLinearPattern(Entity &entity, World &world, const DanmakuPattern &danmakuPattern) {
    entity.addComponent<LinearSpawner>(
        true,
        danmakuPattern.isFanPattern,
        danmakuPattern.bulletSpeed,
        danmakuPattern.speedMultiplier,
        danmakuPattern.bulletPositions,
        danmakuPattern.frequency,
        [&world, &entity, danmakuPattern](Vector2D pos, Vector2D dir, float speed) {
            if (!entity.isActive()) {
                return;
            }
            auto& bullet = world.createDeferredEntity();
            bullet.addComponent<Transform>(pos, 0.0f, 1.0f);
            bullet.addComponent<Velocity>(dir, speed, false);
            bullet.addComponent<ProjectileTag>();

            // TODO: If multiple bullet types are implemented, should probably move this to a helper method.
            SDL_Texture* tex = TextureManager::load("../asset/bullet4.png");
            SDL_FRect src {192, 0, 64, 64};
            SDL_FRect dst {pos.x, pos.y, 32, 32};
            bullet.addComponent<Sprite>(tex, src, dst);

            auto& bulletCol = bullet.addComponent<Collider>("projectile");
            bulletCol.rect.w = dst.w / 2.5f;
            bulletCol.rect.h = dst.h / 2.5f;

            bulletCol.offset.x = (dst.w - bulletCol.rect.w) / 2.0f;
            bulletCol.offset.y = (dst.h - bulletCol.rect.h) / 2.0f;
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
