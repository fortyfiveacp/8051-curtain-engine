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
    timelineEntity.addComponent<StageState>();

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

        timelineEntity.getComponent<Timeline>().timeline.emplace_back(startTime, [&world, data]() {
            auto& spawner = world.createDeferredEntity();
            spawner.addComponent<Convoy>(data);
        });
    }
}

EnemyType StageLoader::stringToEnemyType(const std::string &name) {
    if (name == "SmallFairy") {
        return EnemyType::SmallFairy;
    }

    if (name == "LargeFairy") {
        return EnemyType::LargeFairy;
    }

    return EnemyType::Boss;
}
