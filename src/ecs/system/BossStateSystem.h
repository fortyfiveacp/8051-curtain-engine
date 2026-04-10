#pragma once
#include <memory>
#include <vector>

#include "Entity.h"
#include "Component.h"

class World;

class BossStateSystem {
public:
    void update(World& world, float dt);

private:
    void transitionToPhase(Entity *bossEntity, const PhaseData &phase, World &world);
    bool decrementPhase(Entity* bossEntity, Boss& boss, World& world);

    void checkPhaseTransitions(Entity* entity, Boss& boss, World& world);
    void handleBossDeath(Entity* entity, Boss& boss, World& world);

    void initWinScreen(Entity *entity, const std::vector<std::unique_ptr<Entity>> &entities);

    void resetBossState(Entity* entity);
    void initPhasePatterns(Entity* entity, World& world, const PhaseData& phase);
    void clearScreenProjectiles(World& world);

    void deactivateDanmakuSpawners(Entity *entity);
};