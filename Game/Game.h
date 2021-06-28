#pragma once
#include "GameGrid.h"
#include "CircadianCycle.h"
#include "PathFollower.h"

#include <Sprocket.h>

#include <memory>
#include <random>
#include <string_view>

enum class Mode { PLAYER, EDITOR };

class Game
{
    spkt::Window*      d_window;
    spkt::AssetManager d_assetManager;
    
    Mode d_mode;

    spkt::Scene d_scene;

    spkt::entity d_camera;
    spkt::entity d_worker;
    
    // RENDERING
    spkt::Scene3DRenderer  d_entityRenderer;
    spkt::PostProcessor   d_postProcessor;

    // Additional world setup
    spkt::CircadianCycle d_cycle;

    spkt::ShadowMap d_shadowMap;

    spkt::SimpleUI d_hoveredEntityUI;
    
    // LAYER DATA
    bool d_paused = false;
    std::string d_sceneFile = "";

    // Editor UI
    spkt::DevUI d_devUI;

    // Escape Menu
    spkt::SimpleUI d_escapeMenu;

public:
    Game(spkt::Window* window);

    void on_event(spkt::ev::Event& event);
    void on_update(double dt);
    void on_render();

    void load_scene(std::string_view file);

    void AddRockBase(const glm::ivec2& pos,
                     std::string_view tex,
                     std::string_view name);
    
    void AddRock(const glm::ivec2& pos);
    void AddIron(const glm::ivec2& pos);
    void AddTin(const glm::ivec2& pos);
    void AddMithril(const glm::ivec2& pos);
};