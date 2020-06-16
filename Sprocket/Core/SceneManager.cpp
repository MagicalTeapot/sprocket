#include "SceneManager.h"
#include "Log.h"

namespace Sprocket {

SceneManager::SceneManager()
    : d_scenes()
{

}

Scene* SceneManager::AddScene(const std::string& name)
{
    auto scene = d_scenes.insert(std::make_pair(name, std::make_unique<Scene>()));

    if (scene.second == false) {
        SPKT_LOG_ERROR("A scene with that name already exists!");
        return nullptr;
    }

    return scene.first->second.get();
}

bool SceneManager::SetActiveScene(const std::string& name)
{
    d_activeSceneName = name;
    return true;
}

bool SceneManager::DoesSceneExist(const std::string& name) const
{
    return d_scenes.find(name) != d_scenes.end();
}

void SceneManager::OnUpdate(double dt)
{
    d_scenes[d_activeSceneName]->OnUpdate(dt);
}

void SceneManager::OnEvent(Event& event)
{
    d_scenes[d_activeSceneName]->OnEvent(event);
}

}