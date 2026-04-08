#include "BossStateSystem.h"

#include "ComponentUtils.h"
#include "World.h"
#include "DanmakuFactory.h"

void BossStateSystem::update(World& world, float dt) {
    for (auto& entity : world.getEntities()) {
        if (!entity->hasComponent<Boss>() || !entity->hasComponent<Children>()) {
            continue;
        }

        auto& boss = entity->getComponent<Boss>();
        if (!boss.introCompleted) {
            continue;
        }

        boss.currentHealth = std::max(boss.currentHealth, 0);

        if (boss.isInvulnerable) {
            boss.invulnerabilityTimer -= dt;
            if (boss.invulnerabilityTimer <= 0.0f) {
                boss.isInvulnerable = false;
                boss.invulnerabilityTimer = 0.0f;
            }
            continue;
        }

        if (boss.phaseList.empty()) {
            continue;
        }

        const PhaseData& nextPhase = boss.phaseList.front();
        float healthPercent = static_cast<float>(boss.currentHealth) / static_cast<float>(boss.maxHealth);

        if (boss.currentHealth <= 0 && nextPhase.triggerType == PhaseTrigger::Death) {
            tryConsumePhase(entity.get(), boss, world);
        } else if (nextPhase.triggerType == PhaseTrigger::HealthThreshold && healthPercent <= nextPhase.healthThreshold) {
            transitionToPhase(entity.get(), boss, nextPhase, world);
            boss.phaseList.erase(boss.phaseList.begin());
        }
    }
}

bool BossStateSystem::tryConsumePhase(Entity* bossEntity, Boss& boss, World& world) {
    if (boss.phasesLeft <= 0 || boss.phaseList.empty()) {
        return false;
    }

    const PhaseData& nextPhase = boss.phaseList.front();

    boss.currentHealth = boss.maxHealth;
    boss.phasesLeft--;

    transitionToPhase(bossEntity, boss, nextPhase, world);
    boss.phaseList.erase(boss.phaseList.begin());

    return true;
}

void BossStateSystem::transitionToPhase(Entity* bossEntity, Boss& boss, const PhaseData& phase, World& world) {
    boss.isInvulnerable = true;
    boss.invulnerabilityTimer = Boss::INVULNERABLE_DURATION;

    if (!boss.movementPoints.empty()) {
        boss.targetPoint = boss.originPoint;
        boss.movementTimer = 0.0f;
    }

    if (bossEntity->hasComponent<Timeline>()) {
        ComponentUtils::resetTimeline(bossEntity->getComponent<Timeline>());
    }
    ComponentUtils::deactivateSpawners(bossEntity);

    auto& children = bossEntity->getComponent<Children>();
    for (Entity* child : children.children) {
        if (!child) {
            continue;
        }

        ComponentUtils::deactivateSpawners(child);
    }

    if (!phase.patterns.empty()) {
        if (phase.target == PatternTarget::Boss) {
            DanmakuFactory::buildDanmaku(*bossEntity, world, phase.patterns[0]);
        } else {
            for (size_t i = 0; i < children.children.size(); ++i) {
                Entity* child = children.children[i];
                if (!child || phase.patterns.empty()) {
                    continue;
                }

                if (child->hasComponent<Timeline>()) {
                    ComponentUtils::resetTimeline(child->getComponent<Timeline>());
                }

                const auto& pattern = phase.patterns[i % phase.patterns.size()];
                DanmakuFactory::buildDanmaku(*child, world, pattern);

                if (pattern.shouldTargetPlayer) {
                    for (auto& e : world.getEntities()) {
                        if (e->hasComponent<PlayerTag>()) {
                            auto& playerTransform = e->getComponent<Transform>();
                            child->addComponent<LookAtRotator>(playerTransform, 0.0f);
                            break;
                        }
                    }
                }
            }
        }
    }

    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<ProjectileTag>()) {
            entity->destroy();
        }
    }
}