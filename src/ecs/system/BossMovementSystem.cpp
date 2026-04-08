#include "BossMovementSystem.h"
#include "World.h"
#include "TransformUtils.h"
#include <random>

void BossMovementSystem::update(World &world, float dt) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<Boss>() && entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
            auto& boss = entity->getComponent<Boss>();
            auto& transform = entity->getComponent<Transform>();
            auto& velocity = entity->getComponent<Velocity>();

            Vector2D toTarget = boss.targetPoint - transform.position;
            float distance = toTarget.length();

            if (distance > 2.0f) {
                velocity.direction = toTarget;
                velocity.direction.normalize();

                velocity.currentSpeed = boss.introCompleted ? (boss.movementSpeed * 0.5f) : boss.movementSpeed;
            } else {
                velocity.direction = {0.0f, 0.0f};
                velocity.currentSpeed = 0.0f;

                if (!boss.introCompleted) {
                    TransformUtils::setPosition(entity, boss.targetPoint);
                    boss.introCompleted = true;
                    boss.movementTimer = 0.0f;
                }
            }

            if (velocity.currentSpeed > 0.0f) {
                Vector2D newPos = transform.position + (velocity.direction * velocity.currentSpeed * dt);
                TransformUtils::setPosition(entity, newPos);
            }

            if (boss.introCompleted && boss.movementPoints.size() > 1) {
                boss.movementTimer += dt;

                if (boss.movementTimer >= boss.movementInterval) {
                    boss.movementTimer = 0.0f;

                    auto it = std::find(boss.movementPoints.begin(), boss.movementPoints.end(), boss.targetPoint);
                    size_t currentIndex = (it != boss.movementPoints.end()) ? std::distance(boss.movementPoints.begin(), it) : 0;

                    std::uniform_int_distribution<size_t> dist(0, boss.movementPoints.size() - 2);
                    size_t randomIndex = dist(gen);

                    if (randomIndex >= currentIndex) {
                        randomIndex++;
                    }

                    boss.targetPoint = boss.movementPoints[randomIndex];
                }
            } else if (!boss.movementPoints.empty()) {
                boss.targetPoint = boss.movementPoints.front();
            }
        }
    }
}