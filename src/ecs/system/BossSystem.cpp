#include "BossSystem.h"
#include "World.h"

#include "BossSystem.h"
#include "World.h"
#include "TransformUtils.h"

void BossSystem::update(World& world, float dt) {
    for (auto& entity : world.getEntities()) {
        // SAFETY: Only process active bosses. This prevents crashes during death.
        if (!entity->isActive() || !entity->hasComponent<Boss>()) continue;

        auto& boss = entity->getComponent<Boss>();
        auto& transform = entity->getComponent<Transform>();
        auto& health = entity->getComponent<EnemyHealth>();

        // 1. Intro Movement Phase
        if (!boss.introComplete) {
            Vector2D dir = boss.targetIntroPosition - transform.position;
            if (dir.length() > 2.0f) {
                Vector2D newPos = transform.position + dir.normalize() * 300.0f * dt;
                TransformUtils::setPosition(entity, newPos);
                health.current = health.max;
            } else {
                boss.introComplete = true;
                applyPattern(world, entity.get(), boss, false);
            }
            continue;
        }

        boss.currentHealth = health.current;

        // 2. Invulnerability & Threshold Logic
        if (boss.isInvulnerable) {
            boss.invulnerabilityTimer -= dt;
            if (boss.invulnerabilityTimer <= 0.0f) {
                boss.isInvulnerable = false;
            } else {
                // Lock health during transition
                health.current = boss.inThresholdPhase ?
                    (boss.phases[boss.currentPhaseIndex].thresholdPercentage * health.max) : health.max;
                continue;
            }
        }

        // 3. Phase Transition / Death (FIXED: Safe exit after destruction)
        if (health.current <= 0) {
            if (boss.phasesLeft <= 1) {
                die(entity.get(), boss);
                continue; // Stop processing this entity immediately
            }
            boss.phasesLeft--;
            boss.currentPhaseIndex++;
            boss.inThresholdPhase = false;
            health.current = health.max;
            triggerInvulnerability(boss);
            applyPattern(world, entity.get(), boss, false);
        }
    }
}

void BossSystem::triggerInvulnerability(Boss& boss) {
    boss.isInvulnerable = true;
    boss.invulnerabilityTimer = 1.0f; // 1 second as requested
}

void BossSystem::applyPattern(World& world, Entity* bossEntity, Boss& boss, bool isThreshold) {
    // Clear old spawners from boss and all children
    clearSpawners(bossEntity);
    for (auto* child : boss.childrenEntities) {
        clearSpawners(child);
    }

    const auto& phase = boss.phases[boss.currentPhaseIndex];
    const auto& patternData = isThreshold ? phase.thresholdPattern : phase.normalPattern;
    int targetNodeId = isThreshold ? phase.thresholdNodeId : phase.normalNodeId;

    // Determine who gets the spawner component
    Entity* targetEntity = (targetNodeId == -1) ? bossEntity : boss.childrenEntities[targetNodeId];

    // Because transitions are dynamic, we bypass Timelines and apply directly
    if (patternData.danmakuType == DanmakuType::Radial) {
        DanmakuFactory::initRadialPattern(*targetEntity, world, patternData);
    } else {
        DanmakuFactory::initLinearPattern(*targetEntity, world, patternData);
    }
}

void BossSystem::clearSpawners(Entity* e) {
    if (e->hasComponent<RadialSpawner>()) {
        e->getComponent<RadialSpawner>().isActive = false;
        // Depending on your ECS, you might want to fully remove the component here
        // e->removeComponent<RadialSpawner>();
    }
    if (e->hasComponent<LinearSpawner>()) {
        e->getComponent<LinearSpawner>().isActive = false;
        // e->removeComponent<LinearSpawner>();
    }
}

void BossSystem::die(Entity* bossEntity, Boss& boss) {
    // Clear children
    for (auto* child : boss.childrenEntities) {
        child->destroy();
    }
    // Drop loot, play sound, trigger level end event
    bossEntity->destroy();
}
