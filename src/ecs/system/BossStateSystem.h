#pragma once
#include "Entity.h"
#include "Component.h"

class World;

class BossStateSystem {
public:
    void update(World& world, float dt);

private:
    void transitionToPhase(Entity* bossEntity, Boss& boss, const PhaseData& phase, World& world);
    bool decrementPhase(Entity* bossEntity, Boss& boss, World& world);
};