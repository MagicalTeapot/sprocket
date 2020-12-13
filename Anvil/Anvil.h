#pragma once
#include <Sprocket.h>
#include "Camera.h"

#include "Inspector.h"

#include <memory>
#include <random>

namespace Sprocket {

class Anvil
{
    Window*      d_window;
    AssetManager d_assetManager;
    
    Camera d_editorCamera;

    // Rendering
    EntityRenderer d_entityRenderer;
    SkyboxRenderer d_skyboxRenderer;
    ColliderRenderer d_colliderRenderer;

    ParticleManager d_particleManager;

    FrameBuffer d_viewport;
    DevUI d_ui;
    bool d_isViewportHovered = false;
    bool d_isViewportFocused = false;

    // Scene
    std::string d_sceneFile = "Resources/Anvil.yaml";
    std::shared_ptr<Scene> d_activeScene;
    std::shared_ptr<Scene> d_scene;
    ECS::Entity d_runtimeCamera;

    // Additional world setup
    CubeMap d_skybox;
    float  d_sunAngle = 45.0f;
    
    // LAYER DATA
    bool d_paused = false;
    bool d_mouseRequired = false;

    bool d_playingGame = false;
    bool d_showColliders = false;

    void AddEntityToList(const ECS::Entity& entity);
    ECS::Entity d_selected;

    // Panels
    Inspector      d_inspector;

    void MaterialUI(std::string& texture);

public:
    Anvil(Window* window);

    void OnEvent(Event& event);
    void OnUpdate(double dt);
    void OnRender();

    ECS::Entity Selected() { return d_selected; }
    void SetSelected(ECS::Entity e) { d_selected = e; }
    void ClearSelected() { d_selected = ECS::Null; }

    bool IsGameRunning() const { return d_playingGame; }

    Camera& GetEditorCamera() { return d_editorCamera; }
    Window* GetWindow() { return d_window; }

    std::shared_ptr<Scene> GetScene() { return d_activeScene; }
};

}