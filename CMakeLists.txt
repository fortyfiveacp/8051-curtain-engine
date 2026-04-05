cmake_minimum_required(VERSION 4.0)
project(CurtainEngine)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILES
        src/main.cpp
        src/Game.cpp
        src/manager/TextureManager.h
        src/manager/TextureManager.h
        src/manager/TextureManager.cpp
        src/Map.h
        src/Map.cpp
        src/GameObject.h
        src/GameObject.cpp
        src/ecs/ComponentType.h
        src/ecs/Entity.h
        src/ecs/Component.h
        src/ecs/system/MovementSystem.h
        src/ecs/system/RenderSystem.h
        src/ecs/World.h
        src/utils/Vector2D.h
        src/ecs/system/KeyboardInputSystem.h
        src/utils/Vector2D.cpp
        src/utils/collision.h
        src/utils/Collision.cpp
        src/ecs/system/CollisionSystem.h
        src/ecs/system/CollisionSystem.cpp
        src/ecs/event/EventManager.h
        src/vendor/tinyxml2.cpp
        src/ecs/system/AnimationClip.h
        src/manager/AssetManager.h
        src/manager/AssetManager.cpp
        src/ecs/system/AnimationSystem.h
        src/ecs/system/CameraSystem.h
        src/ecs/system/SpawnTimerSystem.h
        src/ecs/system/DestructionSystem.h
        src/scene/Scene.h
        src/scene/Scene.cpp
        src/manager/SceneManager.h
        src/ecs/event/BaseEvent.cpp
        src/ecs/event/BaseEvent.h
        src/ecs/system/EventResponseSystem.h
        src/ecs/system/EventResponseSystem.cpp
        src/scene/SceneType.h
        src/ecs/system/MainMenuSystem.h
        src/ecs/system/MainMenuSystem.cpp
        src/ecs/system/TimelineSystem.h
        src/ecs/system/RadialSpawnerSystem.h
        src/ecs/system/LinearSpawnerSystem.h
        src/ecs/system/UIRenderSystem.cpp
        src/manager/AudioManager.h
        src/manager/AudioManager.cpp
        src/ecs/event/AudioEventQueue.h
        src/utils/ItemFactory.cpp
        src/ecs/system/SelectableUISystem.cpp
        src/ecs/system/PauseMenuSystem.cpp
        src/ecs/system/DebugRenderSystem.cpp
        src/utils/UIUtils.cpp
        src/utils/StageUtils.cpp
        src/ecs/system/PlayerRespawnSystem.h
        src/ecs/system/PlayerAbilitySystem.cpp
        #add all new source files here
)

if (WIN32)
    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_WIN32")
    set(SDL3_INCLUDE_DIR "${SDL3_SDK_ROOT}/include")
    set(SDL3_LIB_DIR "${SDL3_SDK_ROOT}/lib")

    file(GLOB SDL3_DLLS "${SDL3_SDK_ROOT}/bin/*.dll")
    file(COPY ${SDL3_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

endif ()

if (APPLE)
    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_APPLE")
    set(SDL3_INCLUDE_DIR "${SDL3_SDK_ROOT}/include")
    set(SDL3_LIB_DIR "${SDL3_SDK_ROOT}/lib")
endif ()

include_directories(
        ${SDL3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/ecs
        ${PROJECT_SOURCE_DIR}/src/ecs/system
        ${PROJECT_SOURCE_DIR}/src/utils
        ${PROJECT_SOURCE_DIR}/src/vendor
        #add all new directories here
)
link_directories(
        ${SDL3_LIB_DIR}
)

add_executable(CurtainEngine ${SRC_FILES})

target_link_libraries(CurtainEngine
        PRIVATE
        SDL3
        SDL3_image
        SDL3_ttf
        SDL3_mixer
)

if (APPLE)
    set_target_properties(CurtainEngine PROPERTIES
            BUILD_RPATH "${SDL3_LIB_DIR}"
    )

    target_compile_options(CurtainEngine PRIVATE -Wno-narrowing)
endif ()