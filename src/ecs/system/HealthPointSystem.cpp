#include "HealthPointSystem.h"

#include "ItemFactory.h"
#include "World.h"

void HealthPointSystem::update(World &world) {
    for (auto& entity : world.getEntities()) {
        if (!entity->hasComponent<EnemyHealth>()) {
            continue;
        }

        auto& enemyHealth = entity->getComponent<EnemyHealth>();
        if (enemyHealth.current <= 0) {
            if (entity->hasComponent<LootDropTable>()) {
                auto& loot = entity->getComponent<LootDropTable>();
                Vector2D deathPos = entity->getComponent<Transform>().position;

                for (size_t i = 0; i < loot.types.size(); ++i) {
                    auto& itemEntity = world.createDeferredEntity();
                    Vector2D spawnPos = deathPos + loot.offsets[i];

                    ItemFactory::createItem(itemEntity, loot.types[i], spawnPos);
                }
            }
            AudioManager::playSfx("enemy-dead");
            entity->destroy();
        }
    }
}
