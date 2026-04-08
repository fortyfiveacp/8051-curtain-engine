#include "BossSystem.h"
#include "World.h"

void BossSystem::update(World &world, float dt)  {
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
            } else {
                direction.normalize();
                Vector2D newPos = transform.position + (direction * boss.movementSpeed * dt);
                TransformUtils::setPosition(entity, newPos);
            }
        }
        // TODO: Add phase switching and random movement based on set positions.
    }
}
