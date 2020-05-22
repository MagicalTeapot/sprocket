#include <Sprocket.h>
#include "WorldLayer.h"
#include "EditorUI.h"
#include "EscapeMenu.h"

int main()
{
    Sprocket::Log::init();
    Sprocket::Window window("Game!");
    Sprocket::SceneManager sceneManager;
    Sprocket::ModelManager modelManager;

    window.setCallback([&sceneManager](Sprocket::Event& event) {
        sceneManager.handleEvent(event);
    });

    Sprocket::CoreSystems core;
    core.window = &window;
    core.sceneManager = &sceneManager;
    core.modelManager = &modelManager;

    auto world = sceneManager.addScene("World");
    auto worldLayer = world->add<WorldLayer>(core);
    auto editorUi = world->add<EditorUI>(core, worldLayer.get());
    world->add<EscapeMenu>(core, worldLayer.get(), editorUi.get());
    sceneManager.setActiveScene("World");

    Sprocket::FramerateTimer fps; // Helper class to log the fps.

    while (window.running()) {
        window.clear();

        sceneManager.update();
        sceneManager.draw();

        window.onUpdate();
        fps.update();
    }

    return 0;
}
