#pragma once
#include <Anvil/Camera.h>
#include <Anvil/Inspector.h>
#include <Anvil/ecs/ecs.h>
#include <Anvil/ecs/scene.h>

#include <Sprocket/Core/events.h>
#include <Sprocket/Core/Window.h>
#include <Sprocket/Graphics/asset_manager.h>
#include <Sprocket/Graphics/CubeMap.h>
#include <Sprocket/Graphics/frame_buffer.h>
#include <Sprocket/Graphics/Rendering/geometry_renderer.h>
#include <Sprocket/Graphics/Rendering/pbr_renderer.h>
#include <Sprocket/Graphics/Rendering/SkyboxRenderer.h>
#include <Sprocket/UI/DevUI.h>

#include <memory>
#include <random>
#include <memory>

namespace anvil {

class app
{
    spkt::window*       d_window;
    spkt::asset_manager d_asset_manager;
    
    Camera d_editor_camera;

    // Rendering
    spkt::pbr_renderer d_entity_renderer;
    spkt::SkyboxRenderer d_skybox_renderer;
    spkt::geometry_renderer d_geometry_renderer;

    spkt::frame_buffer d_viewport;
    glm::ivec2 d_viewport_size;

    spkt::DevUI d_ui;
    bool d_is_viewport_hovered = false;
    bool d_is_viewport_focused = false;

    // Scene
    std::string d_sceneFile = "Resources/Anvil.yaml";
    std::shared_ptr<anvil::scene> d_active_scene;
    std::shared_ptr<anvil::scene> d_scene;
    anvil::entity d_runtime_camera;

    // Additional world setup
    spkt::CubeMap d_skybox;
    float  d_sun_angle = 45.0f;
    
    // LAYER DATA
    bool d_paused = false;
    bool d_mouse_required = false;

    bool d_playing_game = false;
    bool d_show_colliders = false;

    anvil::entity d_selected = anvil::null;

    // Panels
    anvil::inspector d_inspector;

    void material_ui(std::string& texture);

public:
    app(spkt::window* window);

    void on_event(spkt::event& event);
    void on_update(double dt);
    void on_render();

    anvil::entity selected() { return d_selected; }
    void set_selected(anvil::entity e) { d_selected = e; }
    void clear_selected() { d_selected = anvil::null; }

    bool is_game_running() const { return d_playing_game; }

    Camera& editor_camera() { return d_editor_camera; }
    spkt::window* window() { return d_window; }

    std::shared_ptr<anvil::scene> active_scene() { return d_active_scene; }
};

}