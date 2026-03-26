#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "../scene/Scene.h"

struct SceneParams {
    SceneType sceneType;
    const char* name;
    const char* mapPath;
    int windowWidth;
    int windowHeight;
};

class SceneManager {
    std::unordered_map<std::string, SceneParams> sceneParams;
    std::string pendingScene;

    void changeScene(const std::string& name) {
        auto it = sceneParams.find(name);
        if (it != sceneParams.end()) {
            const auto& params = it->second;

            // Create Scene object
            // Build scene
            currentScene = std::make_unique<Scene>(
                params.sceneType,
                params.name,
                params.mapPath,
                params.windowWidth,
                params.windowHeight
            );
        }
        else {
            std::cerr << "Scene " << name << " not found" << std::endl;
        }
    }

public:
    std::unique_ptr<Scene> currentScene;
    void loadScene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight) {
        sceneParams[sceneName] = {sceneType, sceneName, mapPath, windowWidth, windowHeight};
    }

    void changeSceneDeferred(const std::string& name) {
        pendingScene = name;
    }

    void update(const float dt, const SDL_Event& e) {
        if (currentScene) {
            currentScene->update(dt, e); // All our world updates
        }

        if (!pendingScene.empty()) {
            changeScene(pendingScene);
            pendingScene.clear();
        }
    }

    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight) const {
        if (currentScene) {
            currentScene->render(renderer, windowWidth, windowHeight);
        }
    }
};
