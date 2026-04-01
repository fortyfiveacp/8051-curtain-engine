#include "ConvoySystem.h"
#include "World.h"
#include "EnemyFactory.h"

void ConvoySystem::update(World& world, float dt) {
    auto& entities = world.getEntities();

    for (auto& entity : entities) {
        if (!entity->hasComponent<Convoy>()) {
            continue;
        }

        auto& convoy = entity->getComponent<Convoy>();
        convoy.timer += dt;

        if (convoy.timer >= convoy.spawnInterval && convoy.numEnemies > 0) {
            spawnEnemy(world, convoy);
            convoy.timer = 0;
            convoy.numEnemies--;

            if (convoy.numEnemies <= 0) {
                entity->destroy();
            }
        }
    }
}

void ConvoySystem::spawnEnemy(World &world, const Convoy &convoy){
    auto& enemy = world.createDeferredEntity();
    EnemyFactory::build(enemy, convoy.enemyType, convoy.pathId, convoy.speed, world);
}
