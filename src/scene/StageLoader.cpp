#include "StageLoader.h"
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
    timelineEntity.addComponent<Timeline>();

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

        auto* danmakuPatternElem = convoyElement->FirstChildElement("Pattern");
        if (danmakuPatternElem) {
            data.danmakuPattern.hasPattern = true;
            data.danmakuPattern.danmakuType = stringToDanmakuType(danmakuPatternElem->Attribute("type"));
            data.danmakuPattern.bulletType = stringToBulletType(danmakuPatternElem->Attribute("bullet"));
            data.danmakuPattern.startTime = danmakuPatternElem->FloatAttribute("startTime");
            data.danmakuPattern.endTime = danmakuPatternElem->FloatAttribute("endTime");
            data.danmakuPattern.frequency = danmakuPatternElem->FloatAttribute("frequency");
            data.danmakuPattern.bulletSpeed = danmakuPatternElem->FloatAttribute("bulletSpeed");

            data.danmakuPattern.bulletsPerBurst = danmakuPatternElem->IntAttribute("bulletsPerBurst");
            data.danmakuPattern.rotationSpeed = danmakuPatternElem->FloatAttribute("rotationSpeed");
            data.danmakuPattern.bulletAngularVel = danmakuPatternElem->FloatAttribute("bulletAngularVel");
            data.danmakuPattern.radius = danmakuPatternElem->FloatAttribute("radius");

            data.danmakuPattern.isFanPattern = danmakuPatternElem->BoolAttribute("isFanPattern");
            data.danmakuPattern.shouldTargetPlayer = danmakuPatternElem->BoolAttribute("targeted");
            data.danmakuPattern.speedMultiplier = danmakuPatternElem->FloatAttribute("speedMultiplier");

            auto* bulletPosRoot = danmakuPatternElem->FirstChildElement("BulletPositions");
            if (bulletPosRoot) {
                for (auto* pos = bulletPosRoot->FirstChildElement("Pos");
                     pos;
                     pos = pos->NextSiblingElement("Pos")) {

                    float posX = pos->FloatAttribute("x");
                    float posY = pos->FloatAttribute("y");
                    data.danmakuPattern.bulletPositions.emplace_back(posX, posY);
                }
            }
        }

        timelineEntity.getComponent<Timeline>().timeline.emplace_back(startTime, [&world, data]() {
            auto& spawner = world.createDeferredEntity();
            spawner.addComponent<Convoy>(data);
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
