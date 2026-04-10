#include "BossStateSystem.h"

#include "ComponentUtils.h"
#include "World.h"
#include "DanmakuFactory.h"
#include "ItemFactory.h"

void BossStateSystem::update(World& world, float dt) {
    for (auto& entity : world.getEntities()) {
        if (!entity->hasComponent<Boss>() || !entity->hasComponent<Children>() || entity->hasComponent<DeadTag>()) {
            continue;
        }

        // Prevent the boss from starting its danmaku patterns until the intro is completed.
        auto& boss = entity->getComponent<Boss>();
        if (!boss.introCompleted) {
            continue;
        }

        // Ensure health doesn't drop below 0
        boss.currentHealth = std::max(boss.currentHealth, 0);

        if (boss.isInvulnerable) {
            boss.invulnerabilityTimer -= dt;
            if (boss.invulnerabilityTimer <= 0.0f) {
                boss.isInvulnerable = false;
                boss.invulnerabilityTimer = 0.0f;
            }
            continue;
        }

        checkPhaseTransitions(entity.get(), boss, world);

        if (boss.currentHealth <= 0) {
            handleBossDeath(entity.get(), boss, world);
        }
    }
}

void BossStateSystem::checkPhaseTransitions(Entity* entity, Boss& boss, World& world) {
    if (boss.phaseList.empty()) {
        return;
    }

    const PhaseData& nextPhase = boss.phaseList.front();
    float healthPercent = static_cast<float>(boss.currentHealth) / static_cast<float>(boss.maxHealth);

    // Transition to the next phase if
    if (nextPhase.triggerType == PhaseTrigger::HealthThreshold && healthPercent <= nextPhase.healthThreshold) {
        transitionToPhase(entity, nextPhase, world);
        boss.phaseList.erase(boss.phaseList.begin());
    }
}

void BossStateSystem::handleBossDeath(Entity* entity, Boss& boss, World& world) {
    if (!boss.phaseList.empty() && boss.phaseList.front().triggerType == PhaseTrigger::Death) {
        decrementPhase(entity, boss, world);
    }
    else if (boss.phasesLeft <= 0 || boss.phaseList.empty()) {
        boss.phasesLeft = 0;
        entity->addComponent<DeadTag>();

        // An inelegant solution to making the boss disappear on death, as there doesn't seem to be a way to
        // disable both Sprite and Animation components.
        if (entity->hasComponent<Transform>()) {
            entity->getComponent<Transform>().position = { 0.0f, -100.0f };
        }

        deactivateDanmakuSpawners(entity);

        AudioManager::playSfx("boss-dead");
        AudioManager::stopMusic();
        clearScreenProjectiles(world);

        initWinScreen(entity, world.getEntities());
    }
}

void BossStateSystem::initWinScreen(Entity* entity, const std::vector<std::unique_ptr<Entity>>& entities) {
    if (entity->hasComponent<Timeline>()) {
        auto& timeline = entity->getComponent<Timeline>();
        float triggerTime = timeline.currentTime + 2.0f;

        timeline.timeline.emplace_back(triggerTime, [&entities]() {\
            for (auto& e : entities) {
                if (e->hasComponent<WinGameMenuTag>() && e->hasComponent<Toggleable>()) {
                    e->getComponent<Toggleable>().toggle();
                }
            }
            AudioManager::playMusic("credits-theme");
        });
    }
}

bool BossStateSystem::decrementPhase(Entity* bossEntity, Boss& boss, World& world) {
    if (boss.phasesLeft <= 0 || boss.phaseList.empty()) {
        return false;
    }

    // Load the next phase.
    const PhaseData& nextPhase = boss.phaseList.front();
    boss.currentHealth = boss.maxHealth;
    boss.phasesLeft--;

    // Transition to the next phase, then remove it from the boss' phase list to avoid duplicate processing.
    transitionToPhase(bossEntity, nextPhase, world);
    boss.phaseList.erase(boss.phaseList.begin());

    return true;
}

// For each phase the boss has, reset its danmaku patterns and clear all existing danmaku projectiles on the screen.
void BossStateSystem::transitionToPhase(Entity* bossEntity, const PhaseData& phase, World& world) {
    resetBossState(bossEntity);
    AudioManager::playSfx("boss-transition");
    initPhasePatterns(bossEntity, world, phase);
    clearScreenProjectiles(world);
}

void BossStateSystem::resetBossState(Entity* entity) {
    auto& boss = entity->getComponent<Boss>();
    boss.isInvulnerable = true;
    boss.invulnerabilityTimer = Boss::INVULNERABLE_DURATION;

    // Move to the origin point and reset the movement timer, as long as the boss
    // has other points to move to.
    if (!boss.movementPoints.empty()) {
        boss.targetPoint = boss.originPoint;
        boss.movementTimer = 0.0f;
    }

    // Reset the boss' danmaku timeline.
    if (entity->hasComponent<Timeline>()) {
        ComponentUtils::resetTimeline(entity->getComponent<Timeline>());
    }

    deactivateDanmakuSpawners(entity);
}

void BossStateSystem::initPhasePatterns(Entity* entity, World& world, const PhaseData& phase) {
    if (phase.patterns.empty()) {
        return;
    }

    auto& children = entity->getComponent<Children>();

    // Build the Danmaku pattern for the boss.
    if (phase.target == DanmakuPatternTarget::Boss) {
        DanmakuFactory::buildDanmaku(*entity, world, phase.patterns[0]);
    } else {
        // Build the Danmaku pattern for the emitter children.
        for (size_t i = 0; i < children.children.size(); ++i) {
            Entity* child = children.children[i];
            if (!child) {
                continue;
            }

            // Reset timelines for each child to allow for new danmaku patterns.
            if (child->hasComponent<Timeline>()) {
                ComponentUtils::resetTimeline(child->getComponent<Timeline>());
            }

            // Initialize a danmaku pattern to every child.
            const auto& pattern = phase.patterns[i % phase.patterns.size()];
            DanmakuFactory::buildDanmaku(*child, world, pattern);

            // For Linear patterns, find the player and "look towards" them.
            if (pattern.shouldTargetPlayer) {
                for (auto& e : world.getEntities()) {
                    if (e->hasComponent<PlayerTag>()) {
                        auto& playerTransform = e->getComponent<Transform>();
                        child->addComponent<LookAtRotator>(&playerTransform, 0.0f);
                        break;
                    }
                }
            }
        }
    }
}

void BossStateSystem::clearScreenProjectiles(World& world) {
    // Find the player.
    Entity* playerEntity = nullptr;
    for (auto& e : world.getEntities()) {
        if (e->hasComponent<PlayerTag>() && e->hasComponent<Transform>()) {
            playerEntity = e.get();
            break;
        }
    }

    // Destroy all bullets.
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<ProjectileTag>()) {
            // If the player was found and the bullet has a transform, create a star item at its location that homes to
            // the player.
            if (playerEntity != nullptr && entity->hasComponent<Transform>()) {
                auto& itemEntity = world.createDeferredEntity();
                ItemFactory::createItem(itemEntity, Star, entity->getComponent<Transform>().position);
                ItemUtils::enableItemHoming(itemEntity, *playerEntity);
            }

            entity->destroy();
        }
    }
}

void BossStateSystem::deactivateDanmakuSpawners(Entity* entity) {
    ComponentUtils::deactivateSpawners(entity);

    if (entity->hasComponent<Children>()) {
        auto& children = entity->getComponent<Children>();
        for (Entity* child : children.children) {
            ComponentUtils::deactivateSpawners(child);
        }
    }
}