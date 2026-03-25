#include "Map.h"

#include <cmath>
#include <iostream>

#include "manager/TextureManager.h"
#include <sstream>
#include <tinyxml2.h>

void Map::load(const char* path, SDL_Texture* ts) {
    tileset = ts;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    // Parse width and height of map
    auto* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    // Parse terrain data
    auto* layer = mapNode->FirstChildElement("layer");
    auto* data = layer->FirstChildElement("data");
    std::string csv = data->GetText();
    std::stringstream ss(csv);
    tileData = std::vector(height, std::vector<int>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::string val;

            // Read characters from a ss into val until it hits a comma, or is at the end of the stream
            if (!std::getline(ss, val, ',')) {
                break;
            }

            tileData[i][j] = std::stoi(val); // stoi is a string to integer converter
        }
    }

    tinyxml2::XMLElement* colliderGroup = nullptr;
    tinyxml2::XMLElement* itemSpawnGroup = nullptr;


    // Find the collider and item spawn objectgroups
    for (auto* obj = layer->NextSiblingElement("objectgroup");
        obj != nullptr;
        obj = obj->NextSiblingElement("objectgroup"))
    {
        if (obj->Attribute("name", "Collision Layer")) {
            colliderGroup = obj;
        }
        else if (obj->Attribute("name", "Item Spawn Layer")) {
            itemSpawnGroup = obj;
        }
    }

    // Create a for-loop with initialization, condition, and an increment
    // For colliders
    if (colliderGroup != nullptr) {
        for (auto* obj = colliderGroup->FirstChildElement("object");
            obj != nullptr;
            obj = obj->NextSiblingElement("object"))
        {
            Collider c;
            c.rect.x = obj->FloatAttribute("x");
            c.rect.y = obj->FloatAttribute("y");
            c.rect.w = obj->FloatAttribute("width");
            c.rect.h = obj->FloatAttribute("height");
            colliders.push_back(c);
        }
    }

    // Create a for-loop with initialization, condition, and an increment
    // For items
    if (itemSpawnGroup != nullptr) {
        for (auto* obj = itemSpawnGroup->FirstChildElement("object");
            obj != nullptr;
            obj = obj->NextSiblingElement("object"))
        {
            Collider itemCollider;
            itemCollider.rect.x = obj->FloatAttribute("x");
            itemCollider.rect.y = obj->FloatAttribute("y");
            itemCollider.rect.w = 32;
            itemCollider.rect.h = 32;

            itemColliders.push_back(itemCollider);
            std::cout << itemCollider.rect.x << " " << itemCollider.rect.y << std::endl;
        }
    }
}

void Map::draw(const Camera& cam) {
    SDL_FRect src{}, dest{};

    dest.w = dest.h = 32;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            // Move the tiles or map relative to the camera
            // Convert from world space to screen space
            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);

            switch (type) {
                // Dirt
                case 1:
                    src.x = 0;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;

                // Grass
                case 2:
                    src.x = 32;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;

                // Water
                case 4:
                    src.x = 32;
                    src.y = 32;
                    src.w = 32;
                    src.h = 32;
                    break;

                default:
                    break;
            }

            TextureManager::draw(tileset, &src, &dest);
        }
    }
}