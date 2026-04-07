#include "StageLoader.h"

#include "BossFactory.h"
#include "tinyxml2.h"

void StageLoader::loadStage(const char *path, World &world) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);
    auto* root = doc.FirstChildElement("Stage");

    auto* pathsRoot = root->FirstChildElement("Paths");
    for (auto* pathElem = pathsRoot->FirstChildElement("Path");
        pathElem;
        pathElem = pathElem->NextSiblingElement("Path")) {

        int id = pathElem->IntAttribute("id");
        Path path;

        for (auto* pt = pathElem->FirstChildElement("Point");
            pt;
            pt = pt->NextSiblingElement("Point")) {

            PathPoint point;
            point.position = Vector2D(pt->FloatAttribute("x"), pt->FloatAttribute("y"));
            point.hoverTime = pt->FloatAttribute("waitTime");

            path.points.push_back(point);
        }
        world.getPathLibrary()[id] = path;
    }

    auto& timelineEntity = world.createEntity();
    auto& timelineComp = timelineEntity.addComponent<Timeline>();

    auto* wavesRoot = root->FirstChildElement("Waves");
    for (auto* convoyElement = wavesRoot->FirstChildElement("Convoy");
        convoyElement;
        convoyElement = convoyElement->NextSiblingElement("Convoy")) {
        float startTime = convoyElement->FloatAttribute("start");

        Convoy data;
        data.enemyType = stringToEnemyType(convoyElement->Attribute("enemyType"));
        data.pathId = convoyElement->IntAttribute("pathId");
        data.numEnemies = convoyElement->IntAttribute("numEnemies");
        data.spawnInterval = convoyElement->FloatAttribute("interval");
        data.speed = convoyElement->FloatAttribute("speed");

        // auto* danmakuPatternElem = convoyElement->FirstChildElement("Pattern");
        data.danmakuPattern = readDanmakuPattern(convoyElement->FirstChildElement("Pattern"));

        timelineEntity.getComponent<Timeline>().timeline.emplace_back(startTime, [&world, data]() {
            auto& spawner = world.createDeferredEntity();
            spawner.addComponent<Convoy>(data);
        });
    }

    for (auto* bossElem = wavesRoot->FirstChildElement("Boss");
         bossElem;
         bossElem = bossElem->NextSiblingElement("Boss")) {

        float startTime = bossElem->FloatAttribute("start");

        // Basic Boss Data
        Boss bossData;
        bossData.bossName = bossElem->Attribute("name");
        bossData.maxHealth = bossElem->IntAttribute("health");
        bossData.currentHealth = bossData.maxHealth;
        bossData.phasesLeft = bossElem->IntAttribute("phases");

        // Parse Invisible Children Offsets
        auto* childrenRoot = bossElem->FirstChildElement("Children");
        if (childrenRoot) {
            for (auto* child = childrenRoot->FirstChildElement("Child"); child; child = child->NextSiblingElement("Child")) {
                bossData.childOffsets.push_back({
                    child->FloatAttribute("offsetX"),
                    child->FloatAttribute("offsetY")
                });
            }
        }

        // Parse Phases
        auto* phasesRoot = bossElem->FirstChildElement("Phases");
        if (phasesRoot) {
            for (auto* phaseElem = phasesRoot->FirstChildElement("Phase"); phaseElem; phaseElem = phaseElem->NextSiblingElement("Phase")) {
                BossPhase phase;
                phase.thresholdPercentage = phaseElem->FloatAttribute("threshold", 0.3f);

                // Normal Pattern
                auto* normalWrap = phaseElem->FirstChildElement("NormalPattern");
                if (normalWrap) {
                    phase.normalNodeId = normalWrap->IntAttribute("nodeId", -1);
                    phase.normalPattern = readDanmakuPattern(normalWrap->FirstChildElement("Pattern"));
                }

                // Threshold (Spellcard) Pattern
                auto* thresholdWrap = phaseElem->FirstChildElement("ThresholdPattern");
                if (thresholdWrap) {
                    phase.thresholdNodeId = thresholdWrap->IntAttribute("nodeId", -1);
                    phase.thresholdPattern = readDanmakuPattern(thresholdWrap->FirstChildElement("Pattern"));
                }

                bossData.phases.push_back(phase);
            }
        }

        // Add to Timeline
        timelineComp.timeline.emplace_back(startTime, [&world, bossData]() {
            auto& bossEntity = world.createDeferredEntity();
            // Call the factory we discussed in the previous step
            BossFactory::buildStageBoss(bossEntity, world, bossData);
        });
    }
}

EnemyType StageLoader::stringToEnemyType(const std::string &name) {
    if (name == "SmallBlueFairy") {
        return EnemyType::SmallBlueFairy;
    }

    if (name == "SmallRedFairy") {
        return EnemyType::SmallRedFairy;
    }

    if (name == "LargeFairy") {
        return EnemyType::LargeFairy;
    }

    return EnemyType::Boss;
}

DanmakuType StageLoader::stringToDanmakuType(const std::string &name) {
    if (name == "Linear") {
        return DanmakuType::Linear;
    }

    return DanmakuType::Radial;
}

BulletType StageLoader::stringToBulletType(const std::string &name) {
    // TODO: Implement more bullet types, if we have time.
    if (name == "LargeOrb") {
        return BulletType::LargeOrb;
    }

    return BulletType::Circle;
}

DanmakuPattern StageLoader::readDanmakuPattern(tinyxml2::XMLElement* patternElement) {
    DanmakuPattern danmakuPattern;
    if (!patternElement) return danmakuPattern;

    danmakuPattern.hasPattern = true;
    danmakuPattern.danmakuType = stringToDanmakuType(patternElement->Attribute("type"));
    danmakuPattern.bulletType = stringToBulletType(patternElement->Attribute("bullet"));
    danmakuPattern.startTime = patternElement->FloatAttribute("startTime");
    danmakuPattern.endTime = patternElement->FloatAttribute("endTime");
    danmakuPattern.frequency = patternElement->FloatAttribute("frequency");
    danmakuPattern.bulletSpeed = patternElement->FloatAttribute("bulletSpeed");
    danmakuPattern.bulletsPerBurst = patternElement->IntAttribute("bulletsPerBurst");
    danmakuPattern.rotationSpeed = patternElement->FloatAttribute("rotationSpeed");
    danmakuPattern.bulletAngularVel = patternElement->FloatAttribute("bulletAngularVel");
    danmakuPattern.radius = patternElement->FloatAttribute("radius");

    danmakuPattern.isFanPattern = patternElement->BoolAttribute("isFanPattern");
    danmakuPattern.shouldTargetPlayer = patternElement->BoolAttribute("targeted");
    danmakuPattern.speedMultiplier = patternElement->FloatAttribute("speedMultiplier");

    auto* bulletPosRoot = patternElement->FirstChildElement("BulletPositions");
    if (bulletPosRoot) {
        for (auto* pos = bulletPosRoot->FirstChildElement("Pos"); pos; pos = pos->NextSiblingElement("Pos")) {
            danmakuPattern.bulletPositions.emplace_back(pos->FloatAttribute("x"), pos->FloatAttribute("y"));
        }
    }
    return danmakuPattern;
}
