#pragma once
#include <string>

#include "Component.h"
#include "World.h"

class StageLoader {
public:
    static void loadStage(const char* path, World& world);

private:
    static EnemyType stringToEnemyType(const std::string& name);
};
