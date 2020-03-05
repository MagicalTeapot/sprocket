#include "Log.h"
#include "Window.h"
#include "Loader.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"
#include "Layer.h"
#include "Scene.h"
#include "LayerStack.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"
#include "Core/Initialiser.h"
#include "Terrain.h"
#include "EntityRenderer.h"
#include "TerrainRenderer.h"
#include "DisplayRenderer.h"
#include "SkyboxRenderer.h"

#include <vector>
#include <memory>

namespace Sprocket {

struct BasicSceneInfo
{
    Window* window;
    Camera  camera;

    Skybox skybox;
 
    std::vector<Entity>   entities;
    std::vector<Terrain>  terrains;
    std::vector<Light>    lights;
    std::vector<ModelPtr> models;
    
    bool paused = false;
    
    BasicSceneInfo(Window* window)
        : window(window)
        , skybox(Loader::loadCubeMap({
            "Resources/Textures/Skybox/Skybox_X_Pos.png",
            "Resources/Textures/Skybox/Skybox_X_Neg.png",
            "Resources/Textures/Skybox/Skybox_Y_Pos.png",
            "Resources/Textures/Skybox/Skybox_Y_Neg.png",
            "Resources/Textures/Skybox/Skybox_Z_Pos.png",
            "Resources/Textures/Skybox/Skybox_Z_Neg.png"
        }))
    {}
};

class GameLayer : public Layer
{
    std::shared_ptr<BasicSceneInfo> d_info;

    EntityRenderer  d_entityRenderer;
    TerrainRenderer d_terrainRenderer;
    SkyboxRenderer  d_skyboxRenderer;

public:
    GameLayer(std::shared_ptr<BasicSceneInfo> info) 
        : Layer(Status::NORMAL, false) 
        , d_info(info)
        , d_entityRenderer(info->window)
        , d_terrainRenderer(info->window)
        , d_skyboxRenderer(info->window)
    {
        auto quadModel = Loader::loadModel("Resources/Models/Plane.obj");
        //auto dragonModel = Loader::loadModel("Resources/Models/Dragon.obj");
        auto deagleModel = Loader::loadModel("Resources/Models/Deagle.obj");

        auto space = Loader::loadTexture("Resources/Textures/Space.PNG");
        auto gray = Loader::loadTexture("Resources/Textures/PlainGray.PNG");
        auto shinyGray = Loader::loadTexture("Resources/Textures/PlainGray.PNG");
        
        shinyGray->reflectivity(3);
        shinyGray->shineDamper(5);

        d_info->terrains.push_back(Terrain(space));

        //d_info->entities.push_back(Entity(dragonModel, shinyGray, {0.0f, 0.0f, -1.0f}, Maths::vec3(0.0f), 0.1f));
        d_info->entities.push_back(Entity(quadModel, gray, {0.0f, -1.0f, 0.0f}, Maths::vec3(0.0f), 20));
        d_info->entities.push_back(Entity(deagleModel, shinyGray, {0.0f, 0.0f, 0.0f}, {180.0f, 0.0f, 0.0f}, 1));

        d_info->lights.push_back(Light{{0.0f, 50.0f, 0.0f}, {0.5f, 0.4f, 0.4f}, {1.0f, 0.0f, 0.0f}});
        d_info->lights.push_back(Light{{5.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.3f, 0.0f}});
        d_info->lights.push_back(Light{{-5.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.0f}});
        d_info->lights.push_back(Light{{8.0f, 4.0f, 2.0f}, {0.3f, 0.8f, 0.2f}, {1.0f, 0.3f, 0.0f}});
    }

    bool handleEventImpl(Window* window, const Event& event) override
    {
        return false;
    }

    void updateImpl(Window* window) override
    {
        d_status = d_info->paused ? Status::PAUSED : Status::NORMAL;

        if (d_status == Status::NORMAL) {
            double tick = layerTicker();

            d_info->lights[1].position.z = 5 * std::sin(tick);
            d_info->lights[1].position.x = 5 * std::cos(tick);

            d_info->lights[2].position.z = 6 * std::sin(-1.5f * tick);
            d_info->lights[2].position.x = 6 * std::cos(-1.5f * tick);

            d_info->lights[3].position.z = 6 * std::sin(8.0f * tick);
            d_info->lights[3].position.x = 6 * std::cos(8.0f * tick);
        }

        d_info->camera.move(window, d_status == Status::NORMAL);
    }

    void drawImpl(Window* window) override
    {
        RenderOptions options;
        options.wireframe = window->isKeyDown(Keyboard::F) &&
                            d_status == Status::NORMAL;

        
        for (const auto& entity: d_info->entities) {
            d_entityRenderer.drawMirror(entity,
                                  d_info->camera,
                                  d_info->lights,
                                  d_info->skybox,
                                  options);
        }
        for (const auto& terrain: d_info->terrains) {
            d_terrainRenderer.draw(terrain,
                                   d_info->camera,
                                   d_info->lights,
                                   options);
        }
        d_skyboxRenderer.draw(d_info->skybox,
                              d_info->camera);
    }
};

class UILayer : public Layer
{
    std::shared_ptr<BasicSceneInfo> d_info;

    DisplayRenderer d_displayRenderer;

public:
    UILayer(std::shared_ptr<BasicSceneInfo> info) 
        : Layer(Status::INACTIVE, true)
        , d_info(info)
        , d_displayRenderer(info->window)
    {
        Vertex2DBuffer v = {{0.5f, 0.5f}, {-0.5f, -0.5f}, {-0.5f, 0.5f},
                        {0.5f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}};
        auto tri = Loader::load2DModel(v);
        d_info->models.push_back(tri);
    }

    bool handleEventImpl(Window* window, const Event& event) override
    {
        if (auto e = event.as<Sprocket::KeyboardButtonPressedEvent>()) {
            if (e->key() == Sprocket::Keyboard::ESC) {
                d_info->paused = !d_info->paused;
                window->setCursorVisibility(d_info->paused);
                return true;
            }
        }
        else if (auto e = event.as<MouseButtonPressedEvent>()) {
            auto pos = window->getMousePos();
            if (pos.x < 50) {
                d_info->paused = false;
                window->setCursorVisibility(false);
            }
        }
        return false;
    }

    void updateImpl(Window* window) override
    {
        d_status = d_info->paused ? Status::NORMAL : Status::INACTIVE;
    }

    void drawImpl(Window* window) override
    {
        Sprocket::RenderOptions options;
        options.faceCulling = false;
        options.depthTest = false;
        for (const auto& model: d_info->models) {
            d_displayRenderer.draw(model, options);
        }
    }
};

}

int main(int argc, char* argv[])
{
    Sprocket::Initialiser init;
    Sprocket::Window window;

    auto info = std::make_shared<Sprocket::BasicSceneInfo>(&window);

    Sprocket::LayerStack layerStack;
    layerStack.pushLayer(std::make_shared<Sprocket::GameLayer>(info));
    layerStack.pushLayer(std::make_shared<Sprocket::UILayer>(info));

    Sprocket::Scene scene("Scene", layerStack, &window);

    while (window.running()) {
        window.clear();
        scene.tick();
        window.onUpdate();
    }

    return 0;
}