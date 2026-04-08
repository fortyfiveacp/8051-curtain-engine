#include "BossSystem.h"
#include "World.h"
#include "TransformUtils.h"
#include <random>

void BossSystem::update(World &world, float dt)  {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (auto& entity : world.getEntities()) {
        if (!entity->hasComponent<Boss>() || !entity->hasComponent<Transform>()) continue;

        auto& boss = entity->getComponent<Boss>();
        auto& transform = entity->getComponent<Transform>();

        if (!boss.introCompleted) {
            Vector2D direction = boss.targetPoint - transform.position;
            float distance = direction.length();

            if (distance < 2.0f) {
                TransformUtils::setPosition(entity, boss.targetPoint);
                boss.introCompleted = true;
                boss.movementTimer = 0.0f;
            } else {
                direction.normalize();
                Vector2D newPos = transform.position + (direction * boss.movementSpeed * dt);
                TransformUtils::setPosition(entity, newPos);
            }
            continue;
        }

        boss.movementTimer += dt;

        Vector2D direction = boss.targetPoint - transform.position;
        if (direction.length() > 2.0f) {
            direction.normalize();
            Vector2D newPos = transform.position + (direction * (boss.movementSpeed * 0.5f) * dt);
            TransformUtils::setPosition(entity, newPos);
        }

        if (boss.movementTimer >= boss.movementInterval) {
            boss.movementTimer = 0.0f;

            if (!boss.movementPoints.empty()) {
                std::uniform_int_distribution<size_t> dist(0, boss.movementPoints.size() - 1);
                size_t randomIndex = dist(gen);
                boss.targetPoint = boss.movementPoints[randomIndex];
            }
        }
    }
}
