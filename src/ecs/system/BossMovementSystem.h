#pragma once
#include "Entity.h"
#include "Component.h"
#include <random>
#include <memory>

class World;

class BossMovementSystem {
public:
    void update(World& world, float dt);

private:
    void updateVelocity(std::unique_ptr<Entity>& entity, Boss& boss, Transform& transform, Velocity& velocity);
    void moveToTarget(std::unique_ptr<Entity>& entity, Transform& transform, const Velocity& velocity, float dt);
    void updateTargetPoint(Boss& boss, float dt, std::mt19937& gen);
};