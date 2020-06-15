#include "WorldLayer.h"

namespace {

Sprocket::Material GetTerrainMaterial()
{
    Sprocket::Material dullGray;
    dullGray.texture = Sprocket::Texture("Resources/Textures/Green.PNG");
    return dullGray;
}

}

WorldLayer::WorldLayer(const Sprocket::CoreSystems& core) 
    : Sprocket::Layer(core)
    , d_mode(Mode::PLAYER)
    , d_entityRenderer(core.window)
    , d_postProcessor(core.window->Width(), core.window->Height())
    , d_lens(core.window->AspectRatio())
    , d_entityManager({&d_selector})
    , d_sunAngle(0)
    , d_camera(5.0f)
    , d_gameGrid(&d_entityManager, &d_modelManager)
    , d_shadowMap(core.window, core.window->Width(), core.window->Height())
    , d_shadowMapRenderer(core.window)
{
    using namespace Sprocket;

    d_lights.sun.direction = {-Maths::Sind(d_sunAngle), -Maths::Cosd(d_sunAngle), 30.0f};
    d_lights.sun.colour = {1.0, 1.0, 1.0};
    d_lights.sun.brightness = 0.5f;

    d_lights.ambience.colour = {1.0, 1.0, 1.0};
    d_lights.ambience.brightness = 0.1f;

    d_postProcessor.AddEffect<GaussianVert>();
    d_postProcessor.AddEffect<GaussianHoriz>();
    d_postProcessor.AddEffect<Negative>();

    {
        auto gun = std::make_shared<Entity>();
        gun->Name() = "Deagle";
        gun->Position() = {10.0f, -1.5f, -3.0f};
        gun->Orientation() = Maths::Rotate({1, 0, 0}, 180.0f);
        
        gun->Add<SelectComponent>();

        auto modelData = gun->Add<ModelComponent>();
        modelData->scale = 1.0f;
        modelData->model = ModelManager::LoadModel("Resources/Models/Deagle.obj");
        modelData->material.texture = Texture::White();
        
        d_entityManager.AddEntity(gun);
    }

    {
        auto terrain = std::make_shared<Entity>();
        terrain->Add<SelectComponent>();
        terrain->Name() = "Terrain";
        terrain->Position() = {-25, 0, -25};
        auto modelData = terrain->Add<ModelComponent>();
        modelData->scale = 1.0f;
        modelData->model = MakeTerrain(51, 1.0f);
        modelData->material = GetTerrainMaterial();
        //terrain->Position() = {0, 0, 0};
        
        d_entityManager.AddEntity(terrain);
    }
}

void WorldLayer::OnEvent(Sprocket::Event& event)
{
    using namespace Sprocket;
    d_mouse.OnEvent(event);

    if (auto e = event.As<WindowResizeEvent>()) {
        d_postProcessor.SetScreenSize(e->Width(), e->Height()); 
        SPKT_LOG_INFO("Resizing!");
    }

    d_camera.OnEvent(event);
    d_lens.OnEvent(event);
    d_entityManager.OnEvent(event);
    d_gameGrid.OnEvent(event);
}

void WorldLayer::OnUpdate(float dt)
{
    using namespace Sprocket;

    d_gameGrid.OnUpdate(d_core.window, &d_camera, &d_lens);
    d_mouse.OnUpdate();

    d_shadowMap.Bind();
    d_shadowMapRenderer.OnUpdate(d_lights.sun);
    for (const auto& [id, entity] : d_entityManager.Entities()) {
        d_shadowMapRenderer.Draw(*entity);
    }
    d_shadowMap.Unbind();

    d_entityRenderer.OnUpdate(
        d_camera,
        d_lens,
        d_lights,
        d_shadowMap.DepthTexId(),
        Maths::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, -150.0f, 150.0f)
            * Maths::LookAt(-d_lights.sun.direction, {0.0, 0.0, 0.0})   
    );
    

    if (!d_paused) {
        d_lights.sun.direction = {-Maths::Sind(d_sunAngle), -Maths::Cosd(d_sunAngle), 0.1f};
        Maths::Normalise(d_lights.sun.direction);
        d_camera.OnUpdate(dt);
        d_entityManager.OnUpdate(dt);
    }

    if (d_paused) {
        d_postProcessor.Bind();
    }

    d_entityManager.Draw(&d_entityRenderer);

    if (d_paused) {
        d_postProcessor.Unbind();
        d_postProcessor.Draw();
    }
}