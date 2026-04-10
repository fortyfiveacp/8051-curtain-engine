#include "BossMovementSystem.h"
#include "World.h"
#include "TransformUtils.h"
#include <algorithm>
#include <random>

void BossMovementSystem::update(World &world, float dt) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<Boss>() && entity->hasComponent<Transform>()
            && entity->hasComponent<Velocity>() && !entity->hasComponent<DeadTag>()) {
            auto& boss = entity->getComponent<Boss>();
            auto& transform = entity->getComponent<Transform>();
            auto& velocity = entity->getComponent<Velocity>();

            updateVelocity(entity, boss, transform, velocity);
            moveToTarget(entity, transform, velocity, dt);
            updateTargetPoint(boss, dt, gen);
        }
    }
}

void BossMovementSystem::updateVelocity(std::unique_ptr<Entity>& entity, Boss& boss, Transform& transform, Velocity& velocity) {
    Vector2D toTarget = boss.targetPoint - transform.position;
    float distance = toTarget.length();

    // Move towards the target position.
    if (distance > 2.0f) {
        velocity.direction = toTarget;
        velocity.direction.normalize();

        if (boss.introCompleted) {
            velocity.currentSpeed = boss.movementSpeed * 0.5f;
        } else {
            velocity.currentSpeed = boss.movementSpeed;
        }
    } else {
        // Arrived at target position. Stay still for a specified movementInterval
        velocity.direction = {0.0f, 0.0f};
        velocity.currentSpeed = 0.0f;

        // Snap position to the initial targetPoint, then mark intro as complete (starts the boss battle).
        if (!boss.introCompleted) {
            TransformUtils::setPosition(entity, boss.targetPoint);
            boss.introCompleted = true;
            boss.movementTimer = 0.0f;
        }
    }
}

void BossMovementSystem::moveToTarget(std::unique_ptr<Entity>& entity, Transform& transform, const Velocity& velocity, float dt) {
    if (velocity.currentSpeed > 0.0f) {
        Vector2D newPos = transform.position + (velocity.direction * velocity.currentSpeed * dt);
        TransformUtils::setPosition(entity, newPos);
    }
}

void BossMovementSystem::updateTargetPoint(Boss& boss, float dt, std::mt19937& gen) {
    if (boss.introCompleted && boss.movementPoints.size() > 1) {
        boss.movementTimer += dt;

        if (boss.movementTimer >= boss.movementInterval) {
            boss.movementTimer = 0.0f;

            // Find the point the boss is on to avoid re-selecting it.
            const auto it = std::ranges::find(boss.movementPoints, boss.targetPoint);
            size_t currentIndex = 0;
            if (it != boss.movementPoints.end()) {
                currentIndex = std::distance(boss.movementPoints.begin(), it);
            } else {
                currentIndex = 0;
            }

            // Select a point to move to that isn't the current point.
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